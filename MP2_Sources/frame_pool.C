/*
    File: frame_pool.C

    Author: Ningyuan Wang
            Department of Electrical and Computer Engineering
            Texas A&M University
    Date  : 14/09/06

    Description: Management of the Free-Frame Pool.


*/

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/
#include "frame_pool.H"
/*--------------------------------------------------------------------------*/
/* F r a m e   P o o l  */
/*--------------------------------------------------------------------------*/
  /* Initializes the data structures needed for the management of this
      frame pool. This function must be called before the paging system
      is initialized.
      _base_frame_no is the frame number at the start of the physical memory
      region that this frame pool manages.
      _nframes is the number of frames in the physical memory region that this
      frame pool manages.
      e.g. If _base_frame_no is 16 and _nframes is 4, this frame pool manages
      physical frames numbered 16, 17, 18 and 19
      _info_frame_no is the frame number (within the directly mapped region) of
      the frame that should be used to store the management information of the
      frame pool. However, if _info_frame_no is 0, the frame pool is free to
      choose any frame from the pool to store management information.
      */

FramePool::FramePool(unsigned long _base_frame_no,unsigned long _nframes,unsigned long _info_frame_no)
{
	base_frame_no = _base_frame_no;
	nframes = _nframes;
	info_frame_no = _info_frame_no;
	if (info_frame_no == 0)
		info_frame_no = 512;
	used = (char*)(info_frame_no << 12);
	for( unsigned long i = 0; i < ( (nframes - 1)>> 3) ; i++)
		used[i] = 0;
	int offset = nframes & 0x0007;
	char mask = 0;
	for ( int i = 0; i < offset; i++)
		mask = (mask << 1) | 0x1;
	mask = ~mask;
	used[ (nframes - 1)>> 3] = used[ (nframes -1 ) >> 3] & mask;
	mark_inaccessible(info_frame_no,1);
	return;
}
	
   /* Allocates a frame from the frame pool. If successful, returns the frame
    * number of the frame. If fails, returns 0. */

unsigned long FramePool::get_frame()
{
	for( unsigned int i = 0; i <= (nframes >> 3) ; i++)
		if (~used[i] != 0)
		{
			int offset;
			for( offset = 0; offset < 8; offset++)
				if ((  (~used[i]) & (0x1 << offset)) != 0 )
					break;
			if (offset == 8 )
				return 0;
			used[i] = used[i] | ( 0x1 << offset);
			if ( (i << 3) + offset > nframes - 1)
				return 0;
			Console::puts("\nget frame: ");
			Console::putui(base_frame_no + (i << 3 )+ offset);
			Console::puts("\n");
			mark_inaccessible_frame( i << 3 + offset);
			return base_frame_no + (i << 3) + offset;
		}
	return 0;
}


   /* Mark the area of physical memory as inaccessible. The arguments have the
    * same semanticas as in the constructor.
    */

void FramePool::mark_inaccessible(unsigned long _base_frame_no,unsigned long _nframes)
{
	for( unsigned long i = base_frame_no - _base_frame_no; i <  base_frame_no - _base_frame_no + _nframes; i++)
		mark_inaccessible_frame(i);
	return;
}
void inline FramePool::mark_inaccessible_frame(unsigned long frame_index)
{
	unsigned long start = frame_index >> 3;
	int offset_index = frame_index & 0x7;
	char mask = 1 << offset_index;
	used[start] = used[start] | mask;
	return;
}

   /* Releases frame back to the given frame pool.
      The frame is identified by the frame number. 
      NOTE: This function is static because there may be more than one frame pool
      defined in the system, and it is unclear which one this frame belongs to.
      This function must first identify the correct frame pool and then call the frame
      pool's release_frame function. */
void FramePool::release_frame(unsigned long _frame_no)
{
	char* map;
	int index;
	if ( _frame_no < 1024 )
	{
		map = (char *)(512 << 12);
		index = _frame_no - 512;
	}
	else
	{
		map = (char *)(1024 << 12);
		index = _frame_no - 1024;
	}
	int offset_index = index  & 0x7;
	char mask = 1 << offset_index;
	mask = ~mask;
	map[index >> 3] = map[index >> 3] & mask;
	return;
}
