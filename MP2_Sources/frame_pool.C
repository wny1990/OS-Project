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
bool FramePool::pool[8096];
unsigned long FramePool::base_frame_no;
unsigned long FramePool::nframes;
unsigned long FramePool::info_frame_no;


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
	FramePool::FramePool(unsigned long _base_frame_no,
             unsigned long _nframes,
             unsigned long _info_frame_no)
 
	{
		base_frame_no = _base_frame_no;
		nframes = _nframes;
		info_frame_no = _info_frame_no;
		return;
	}
	
   /* Allocates a frame from the frame pool. If successful, returns the frame
    * number of the frame. If fails, returns 0. */
	unsigned long FramePool::get_frame()
	{
		for( int i = 0; i < nframes - info_frame_no; i++)
			if (pool[i] == false)
			{
				pool[i] = true;
				return base_frame_no + i;
			}
		return 0;
	}
	
   /* Mark the area of physical memory as inaccessible. The arguments have the
    * same semanticas as in the constructor.
    */
	void FramePool::mark_inaccessible(unsigned long _base_frame_no,
                          unsigned long _nframes)
	{
		for( int i = _base_frame_no - base_frame_no; i < _base_frame_no - base_frame_no + _nframes; i++)
			pool[i] = true;
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
		pool[_frame_no - base_frame_no] = false;
		return;
	}
