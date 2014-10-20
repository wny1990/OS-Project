/*
    File: vm_pool.C

    Author: Ningyuan Wang
            Department of Electrical and Computer Engineering
            Texas A&M University
    Date  : 09/24/2014

    Description: Management of the Virtual Memory Pool


*/
#include "vm_pool.H"
#include "console.H"
#include "page_table.H"

VMPool::VMPool(unsigned long _base_address,
          unsigned long _size,
          FramePool *_frame_pool,
          PageTable *_page_table)
   /* Initializes the data structures needed for the management of this
    * virtual-memory pool.
    * _base_address is the logical start address of the pool.
    * _size is the size of the pool in bytes.
    * _frame_pool points to the frame pool that provides the virtual
    * memory pool with physical memory frames.
    * _page_table points to the page table that maps the logical memory
    * references to physical addresses. */
{
	base_address = _base_address;
	size = _size;
	frame_pool = _frame_pool;
        page_table =_page_table;
	_page_table->_register(this);
	regions = (unsigned long *) ((_page_table->kernel_mem_pool->get_frame() ) << 12);
	c_regions = 0;
	return;
}   
unsigned long VMPool::allocate(unsigned long _size)
   /* Allocates a region of _size bytes of memory from the virtual
    * memory pool. If successful, returns the virtual address of the
    * start of the allocated region of memory. If fails, returns 0. */
{
	unsigned long start = base_address;
	int found_index = 0;
	// extend size to page size
	//if (_size % 4096 )
	//	_size = (_size & ~(0xfff) ) + 4096;
	while (_size % 4096 )
		_size++;
	//first fit algorithm, the list is always ascending ordered
	if ( c_regions)
	{
		for ( int index = -2;  index < 2 * c_regions ; index = index + 2)
		{
			long right_addr;
			if (index + 2 >= 2 * c_regions )
				right_addr = base_address + size;
			else
				right_addr = regions[index+2];
			long left_addr; 
			if (index == -2 )
				left_addr = base_address;
			else
				left_addr =  regions[index] + regions[index+1];

			if (  (unsigned int)(right_addr - left_addr) >= _size )
			{
				start = left_addr;
				found_index = index;
				break;
			}
		}

		for ( int i = 2*(c_regions -1); i >= found_index + 2; i = i - 2)
		{
			regions[i - 2] = regions[i];
			regions[i - 3] = regions[i - 1];
		}
	}
	regions[found_index] = start;
	regions[found_index + 1] = _size;
	
	Console::puts("\nallocate address start from: ");
	Console::putui(start);
	Console::puts("\n");
	c_regions++;
	return start;
}   

void VMPool::release(unsigned long _start_address)
   /* Releases a region of previously allocated memory. The region
    * is identified by its start address, which was returned when the
    * region was allocated. */
{

	int index;
	//find the index of the start address.
	for ( index = 0;  index < 2 * c_regions ; index = index + 2)
		if (regions[index] == _start_address)
			break;
	// free page in page table 
	if (index == 2 * c_regions )
	{
		Console::puts("\nRegion to release not found: \n");
		abort();
	}
	unsigned long address = _start_address;
	for ( address  = _start_address; address < regions[index] + regions[index+1]; address += 4096)
	{
		Console::puts("\nrelease address start from: ");
		Console::putui(address);
		Console::puts("\n");
		page_table->free_page(address >> 12);
	}
	// delete the info of this region
	for ( int i = index; i < 2 * (c_regions - 1); i = i + 2)
	{
		regions[i] = regions[i + 2];
		regions[i + 1] = regions[i + 3];
	}
	c_regions--;
	return;
}   

BOOLEAN VMPool::is_legitimate(unsigned long _address)
   /* Returns FALSE if the address is not valid. An address is not valid
    * if it is not part of a region that is currently allocated. */
{
	//if this is a recursive pointer, set the reference valid
	//if( _address >> 22 == 1023 || ((_address >> 12 ) & 0x3ff ) == 1023 )
	//	return true;
	for ( int i = 0;  i < 2 * c_regions ; i = i + 2)
	{
		if( _address >= regions[i] && _address < regions[i] + regions[i+1])
			return true;
	}
	return false;
}   

