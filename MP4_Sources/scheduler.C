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
        q_len = 0;
        return;
    }
    void Scheduler::yield()
    /* Called by the currently running thread in order to give up the CPU. 
      The scheduler selects the next thread from the ready queue to load onto 
      the CPU, and calls the dispatcher function defined in 'threads.h' to
      do the context switch. */
    {
        Thread* target = queue[0];
        for( int i = 0; i < q_len - 1; i++ )
            queue[i] = queue[i + 1];
        q_len--;
        Console::puts("q_len:");
        Console::puti(q_len);
        Console::puts("\n");
        Thread::dispatch_to(target);
        return;
    }

    void Scheduler::resume(Thread * _thread)
    /* Add the given thread to the ready queue of the scheduler. This is called
      for threads that were waiting for an event to happen, or that have 
      to give up the CPU in response to a preemption. */
    {
        queue[q_len] = _thread;
        q_len++;
        return;
    }

    void Scheduler::add(Thread * _thread)
    /* Make the given thread runnable by the scheduler. This function is called
	  typically after thread creation. Depending on the
      implementation, this may not entail more than simply adding the 
      thread to the ready queue (see scheduler_resume). */
    {
        queue[q_len] = _thread;
        q_len++;
        return;
    }

    void Scheduler::terminate(Thread * _thread)
    /* Remove the given thread from the scheduler in preparation for destruction
      of the thread. */
    {
        int target_index = 0;
        for( int i = 0; i < q_len; i++ )
            if ( queue[i] == _thread)
                target_index = i;
        for( int i = target_index; i < q_len - 1; i++ )
            queue[i] = queue[i+1];
        q_len--;
        return;
    }

