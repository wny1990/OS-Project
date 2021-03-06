/* 
    Author: Ningyuan Wang
			Based on code by:
			R. Bettati
            Department of Computer Science
            Texas A&M University
			
			A thread scheduler.

*/

#include "scheduler.H"

/*--------------------------------------------------------------------------*/
/* SCHEDULER */
/*--------------------------------------------------------------------------*/

    Scheduler::Scheduler()
    /* Setup the scheduler. This sets up the ready queue, for example.
      If the scheduler implements some sort of round-robin scheme, then the 
      end_of_quantum handler is installed here as well. */
    {
        ready_queue = Queue<Thread*>();
        blocking_queue = Queue<Thread*>();
        return;
    }
    void Scheduler::yield()
    /* Called by the currently running thread in order to give up the CPU. 
      The scheduler selects the next thread from the ready queue to load onto 
      the CPU, and calls the dispatcher function defined in 'threads.h' to
      do the context switch. */
    {
        Thread* target = ready_queue.dequeue();
     /*
	Console::puts("q_len:");
        Console::puti(queue.get_size());
        Console::puts("\n");
      */
        Thread::dispatch_to(target);
        return;
    }

    void Scheduler::resume(Thread * _thread)
    /* Add the given thread to the ready queue of the scheduler. This is called
      for threads that were waiting for an event to happen, or that have 
      to give up the CPU in response to a preemption. */
    {
        ready_queue.enqueue(_thread);
	if (! blocking_queue.isEmpty())
	{
		//if read continue, if not return
   		if (!inportb(0x1F7) & 0x08)
			return;
		
		Console::puts("blocking_q_len:");
	        Console::puti(blocking_queue.get_size());
	        Console::puts("\n");
	     
      
        	Thread* target = blocking_queue.dequeue();
		//do something
        	ready_queue.enqueue(target);
	}
        return;
    }

    void Scheduler::add(Thread * _thread)
    /* Make the given thread runnable by the scheduler. This function is called
	  typically after thread creation. Depending on the
      implementation, this may not entail more than simply adding the 
      thread to the ready queue (see scheduler_resume). */
    {
        ready_queue.enqueue(_thread);
        return;
    }

    void Scheduler::terminate(Thread * _thread)
    /* Remove the given thread from the scheduler in preparation for destruction
      of the thread. */
    {
        ready_queue.remove(_thread);
        return;
    }

    void Scheduler::blocking(Thread * _thread)
    {
        blocking_queue.enqueue(_thread);

    }

