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
//	regions = (unsigned long *)(_base_address);
//	regions[0] = _base_address;
//	regions[1] = 4 * (1 << 12);
//	c_regions = 1;
	c_regions = 0;
	return;
}   
unsigned long VMPool::allocate(unsigned long _size)
   /* Allocates a region of _size bytes of memory from the virtual
    * memory pool. If successful, returns the virtual address of the
    * start of the allocated region of memory. If fails, returns 0. */
{
	unsigned long max = base_address;
	unsigned long m_size = 0;
	for ( int index = 0;  index < 2 * c_regions ; index = index + 2)
		if (regions[index] > max)
		{
			max = regions[index];
			m_size = regions[index + 1];
		}
	regions[c_regions * 2] = max + m_size;
	regions[c_regions * 2 + 1] = _size;
	Console::puts("\nallocate address start from: ");
	Console::putui(max + m_size);
	Console::puts("\n");
	c_regions++;
	return max + m_size;
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
	unsigned long region_size = regions[index + 1];
	for ( unsigned long  i = 0; i <= (region_size >> 12 ); i++)
		page_table->free_page( (_start_address >> 12 ) + i);

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
	for ( int i = 0;  i < 2 * c_regions ; i = i + 2)
	{
		if( _address >= regions[i] && _address < regions[i] + regions[i+1])
			return true;
	}
	return false;
}   

