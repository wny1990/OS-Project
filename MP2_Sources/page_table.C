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
	// attribute set to: supervisor level, read/write, present(011 in binary)
	for(unsigned int i = 0; i < shared_size / (PAGE_SIZE * ENTRIES_PER_PAGE); i++)
	{
		page_directory[i] = (long unsigned int) (page_table + PAGE_SIZE * i);
		page_directory[i] = page_directory[i] | 3; 
	}
	// attribute set to: supervisor level, read/write, not present(010 in binary)
	for(unsigned int i =  shared_size / (PAGE_SIZE * ENTRIES_PER_PAGE) + 1; i < ENTRIES_PER_PAGE; i++)
		page_directory[i] = 0 | 2; 

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
		return;
	}
	else
		Console::puts("Page not Present\n");
 	unsigned long address = read_cr2();
 	unsigned long *PT;
	//page table not present in page directory
	if( ((current_page_table->page_directory[ address >> 22]) & 0x1 ) == 0 )
	{
		//allocate frame for page table
		PT = (unsigned long *)(kernel_mem_pool->get_frame()<<12);
		for(unsigned int  i = 0; i < ENTRIES_PER_PAGE; i++ )
			PT[i] = 2;
		current_page_table->page_directory[address>>22] = (unsigned long)PT | 3;
	}
	//page table presented in page directory
	else
		PT = (unsigned long *)(current_page_table->page_directory[address>>22]&0xFFFFF000);
	
 	unsigned long frame = process_mem_pool->get_frame();
	if( frame == 0 )
	{
		Console::puts("Run out of Memory.");
		return;
	}
	PT[ (address>>12) & 0x3ff ] = (frame << 12) | 3;
	return;
}
