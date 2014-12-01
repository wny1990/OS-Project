/*
     File        : Blocking_disk.C

     Author      : Ningyuan Wang
     Modified    : 11/28/14

     Description :  
*/

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include"blocking_disk.H"
extern Scheduler * SYSTEM_SCHEDULER;

    //call the constructor of the parent class
    BlockingDisk::BlockingDisk(DISK_ID _disk_id, unsigned int _size): SimpleDisk( _disk_id,  _size)
    {
	return;
    }
   /* DISK OPERATIONS */


    void BlockingDisk::read(unsigned long _block_no, char * _buf)  
   /* Reads 512 Bytes from the given block of the disk and copies them 
      to the given buffer. No error check! */
    {
        issue_operation(READ, _block_no);

        // if it is busy, insert into block queue
        SYSTEM_SCHEDULER->blocking(Thread::CurrentThread());
        // give up the CPU
        SYSTEM_SCHEDULER->yield();

        /* read data from port */
        int i;
        unsigned short tmpw;
        for (i = 0; i < 256; i++) 
        {
            tmpw = inportw(0x1F0);
           _buf[i*2]   = (unsigned char)tmpw;
           _buf[i*2+1] = (unsigned char)(tmpw >> 8);
        }
        //SimpleDisk::read(_block_no,_buf);
        return;
    }
    void BlockingDisk::write(unsigned long _block_no, char * _buf)
    /* Writes 512 Bytes from the buffer to the given block on the disk. */
    {

        issue_operation(WRITE, _block_no);

        // if it is busy, insert into block queue
        SYSTEM_SCHEDULER->blocking(Thread::CurrentThread());
        // give up the CPU
        SYSTEM_SCHEDULER->yield();


        /* write data to port */
        int i; 
        unsigned short tmpw;
        for (i = 0; i < 256; i++) 
        {
            tmpw = _buf[2*i] | (_buf[2*i+1] << 8);
            outportw(0x1F0, tmpw);
        }
        //SimpleDisk::write(_block_no,_buf);
        return;
    }
