/*
    File: page_table.C

    Author: Ningyuan Wang
            Department of Electrical and Computer Engineering
            Texas A&M University
    Date  : 14/09/06

    Description: Basic Paging.

*/

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "page_table.H"

  PageTable     *PageTable::current_page_table; /* pointer to currently loaded page table object */
  unsigned int    PageTable::paging_enabled;     /* is paging turned on (i.e. are addresses logical)? */
  FramePool     *PageTable::kernel_mem_pool;    /* Frame pool for the kernel memory */
  FramePool     *PageTable::process_mem_pool;   /* Frame pool for the process memory */
  unsigned long   PageTable::shared_size;        /* size of shared address space */

	/* Set the global parameters for the paging subsystem. */
void PageTable::init_paging(FramePool * _kernel_mem_pool,FramePool * _process_mem_pool,const unsigned long _shared_size)
{
	paging_enabled = 0;
	kernel_mem_pool = _kernel_mem_pool;
	process_mem_pool = _process_mem_pool;
	shared_size = _shared_size;
	return;
}

	/* Initializes a page table with a given location for the directory and the page table proper.
	NOTE: The PageTable object still needs to be stored somewhere! Probably it is best
	to have it on the stack, as there is no memory manager yet...
	NOTE2: It may also be simpler to create the first page table *before* paging has been enabled.
	*/

PageTable::PageTable()
{
	// 12 means 2^12 = 4096
	page_directory = (unsigned long *)(kernel_mem_pool->get_frame() << 12);
	page_table = (unsigned long *)(kernel_mem_pool->get_frame() << 12);

	unsigned long address = 0;
	// attribute set to: supervisor level, read/write, present(011 in binary)
	for(unsigned int i = 0; i < shared_size / PAGE_SIZE; i++)
	{
		page_table[i] = address | 3; 
		address += PAGE_SIZE;
	}
	page_table[1023] = (unsigned long ) page_table | 3;

	// attribute set to: supervisor level, read/write, present(011 in binary)
	for(unsigned int i = 0; i < shared_size / (PAGE_SIZE * ENTRIES_PER_PAGE); i++)
	{
		page_directory[i] = (unsigned long) (page_table + PAGE_SIZE * i);
		page_directory[i] = page_directory[i] | 3; 
	}
	// attribute set to: supervisor level, read/write, not present(010 in binary)
	for(unsigned int i =  shared_size / (PAGE_SIZE * ENTRIES_PER_PAGE) + 1; i < ENTRIES_PER_PAGE; i++)
		page_directory[i] = 0 | 2; 

	page_directory[1023] = ( unsigned long) page_directory | 3;

	c_vm = 0;
	return;

}
	/* Makes the given page table the current table. This must be done once during
	system startup and whenever the address space is switched (e.g. during
	process switching). */

void PageTable::load()
{
	write_cr3((unsigned long int )page_directory);
	current_page_table = this;
	return;
}

	/* Enable paging on the CPU. Typically, a CPU start with paging disabled, and
	memory is accessed by addressing physical memory directly. After paging is
	enabled, memory is addressed logically. */

void PageTable::enable_paging()
{

	unsigned long cr_0 = read_cr0();
	cr_0 = cr_0 | 0x80000000;
 	paging_enabled = 1; 
	write_cr0(cr_0);
	return;
}
/* The page fault handler. */
void PageTable::handle_fault(REGS * _r)
{

	Console::puts("PAGE FALUT CAUGHT\n");
	if (_r->err_code & 2)
		Console::puts("Write ");
	else
		Console::puts("Read ");
	if (_r->err_code & 1)
	{
		Console::puts("Protection Fault\n");
		abort();
	}
	else
		Console::puts("Page not Present\n");
 
	unsigned long address = read_cr2();
	unsigned long* dir_entry_address =(unsigned long*)( ( 0x3ff << 22 )| ( 0x3ff << 12 ) | (address >> 22) << 2 );
 	unsigned long* entry_address = (unsigned long*) (( 0x3ff << 22 ) | (( address >> 12 ) << 2) );

// check if the logical address legitimate
	bool found = false;
// check every virtual memory pool for that
	for ( int i = 0; i < current_page_table->c_vm; i++)
		if (current_page_table->vm_pool[i]->is_legitimate(address))
			found = true;		
	if ( found == false)
	{
		Console::puts("Memory Reference Invalid.\n");
		Console::putui(address);
		Console::puts("\n.");
		abort();
	}
	//page table not present in page directory
	if( ((*dir_entry_address) & 0x1)  == 0 )
	{
		//allocate frame for page table
		unsigned long frame_phy_address = (process_mem_pool->get_frame() ) << 12;
		*dir_entry_address = (unsigned long ) frame_phy_address | 3;
		unsigned long* new_table = (unsigned long*)( ( 0x3ff << 22) |( (address >> 22 ) << 12 ) );
		for(unsigned int  i = 0; i < ENTRIES_PER_PAGE; i++ )
		{
			*(new_table +  i ) = 2;
			if (i == ENTRIES_PER_PAGE - 1)
				*(new_table + i ) = (unsigned long) frame_phy_address | 3;
		}
	}

 	unsigned long frame = process_mem_pool->get_frame();
	//set the page table entry content
	*entry_address = (frame << 12 ) | 3;

	if( frame == 0 )
	{
		Console::puts("Run out of Memory.");
		abort();
	}
	return;
}
// register a virtual memory pool for this page table ( address space)
void PageTable::_register(VMPool* _pool)
{
	vm_pool[c_vm] = _pool;
	c_vm++;
	return;
}

void  PageTable::free_page(unsigned long _page_no)
{
	unsigned long* entry_address = (unsigned long *)( (0x3ff << 22) | (_page_no << 2) );
	unsigned long phy_address = *entry_address;
	unsigned long frame_no = phy_address >> 12;
	Console::puts("release frame no: ");
	Console::putui(frame_no);
	Console::puts("\n");
	return;
	if ( phy_address & 0x1)
	{
		FramePool::release_frame(frame_no);
		// clear the present bit
		*entry_address = 0x2;
	}
	return;
}

