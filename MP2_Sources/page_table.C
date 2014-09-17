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

	void PageTable::init_paging(FramePool * _kernel_mem_pool,
                          FramePool * _process_mem_pool,
                          const unsigned long _shared_size)
	/* Set the global parameters for the paging subsystem. */
  	{
		paging_enabled = 0;
		kernel_mem_pool = _kernel_mem_pool;
		process_mem_pool = _process_mem_pool;
		shared_size = _shared_size;
		return;
	}

	PageTable::PageTable()
	/* Initializes a page table with a given location for the directory and the page table proper.
	NOTE: The PageTable object still needs to be stored somewhere! Probably it is best
	to have it on the stack, as there is no memory manager yet...
	NOTE2: It may also be simpler to create the first page table *before* paging has been enabled.
	*/
	{
		page_directory =  (long unsigned int *)0x9C000;
		page_table =  (long unsigned int *)0x9D000;
		current_page_table =   this;
		unsigned long address = 0;
		for(int i = 0; i < shared_size / PAGE_SIZE; i++)
		{
			// attribute set to: supervisor level, read/write, present(011 in binary)
			page_table[i] =(long unsigned int) (address | 3); 
			address += PAGE_SIZE;
		};
		page_directory[0] = (long unsigned int)  page_table;
		// attribute set to: supervisor level, read/write, not present(010 in binary)
		for(int i = 1; i < 1024; i++)
			page_directory[i] = 0 | 2; 
		return;
	}

	void PageTable::load()
	/* Makes the given page table the current table. This must be done once during
	system startup and whenever the address space is switched (e.g. during
	process switching). */
	{
		write_cr3((unsigned long int )page_directory);
		return;
	}
	void PageTable::enable_paging()
	/* Enable paging on the CPU. Typically, a CPU start with paging disabled, and
	memory is accessed by addressing physical memory directly. After paging is
	enabled, memory is addressed logically. */
	{
		unsigned long cr_0 = read_cr0();
		cr_0 = cr_0 | 0x80000000;
		write_cr0(cr_0);
 		paging_enabled = 1; 
		return;
	}
	void PageTable::handle_fault(REGS * _r)
	/* The page fault handler. */
	{
		Console::puts("PAGE FALUT CAUGHT\n");
		if (_r->err_code & 2)
			Console::puts("Write");
		else
			Console::puts("Read");
		if (_r->err_code & 1)
			Console::puts("Protection Fault");
		else
			Console::puts("Page not Present");
		Console::puts("\n");
		return;
	}
