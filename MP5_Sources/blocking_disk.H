/*
     File        : Blocking_disk.H

     Author      : Ningyuan Wang
     Modified    : 11/28/14

     Description :  
*/

#ifndef _BLOCKING_DISK_H_
#define _BLOCKING_DISK_H_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include"simple_disk.H"
#include"scheduler.H"
#include"thread.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

//Queue read_q;
//Queue write_q;

/*--------------------------------------------------------------------------*/
/* S i m p l e D i s k  */
/*--------------------------------------------------------------------------*/

class BlockingDisk : public SimpleDisk  {
public:

   BlockingDisk(DISK_ID _disk_id, unsigned int _size); 

   /* DISK OPERATIONS */

   virtual void read(unsigned long _block_no, char * _buf);
   /* Reads 512 Bytes from the given block of the disk and copies them 
      to the given buffer. No error check! */

   virtual void write(unsigned long _block_no, char * _buf);
   /* Writes 512 Bytes from the buffer to the given block on the disk. */

};

#endif
