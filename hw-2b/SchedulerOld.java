import java.util.*; // Scheduler_mfq.java

public class Scheduler extends Thread
{
    @SuppressWarnings({"unchecked","rawtypes"})
    private Vector<TCB>[] queue = new Vector[3];
    private int timeSlice;
    private static final int DEFAULT_TIME_SLICE = 1000;

    // New data added to the original algorithm 
    private boolean[] tids; // Indicate which ids have been used
    private static final int DEFAULT_MAX_THREADS = 10000;

    // A new feature added to the original algorithm 
    // Allocate an ID array, each element indicating if that id has been used
    private int nextId = 0;
    private void initTid( int maxThreads ) {
        tids = new boolean[maxThreads];
        for ( int i = 0; i < maxThreads; i++ )
            tids[i] = false;
    }

    // A new feature added to the original algorithm 
    // Search an available thread ID and provide a new thread with this ID
    private int getNewTid( ) {
        for ( int i = 0; i < tids.length; i++ ) {
            int tentative = ( nextId + i ) % tids.length;
            if ( tids[tentative] == false ) {
                tids[tentative] = true;
                nextId = ( tentative + 1 ) % tids.length;
                return tentative;
            }
        }
        return -1;
    }

    // A new feature added to the original algorithm 
    // Return the thread ID and set the corresponding tids element to be unused
    private boolean returnTid( int tid ) {
        if ( tid >= 0 && tid < tids.length && tids[tid] == true ) {
            tids[tid] = false;
            return true;
        }
        return false;
    }

    // A new feature added to the original algorithm 
    // Retrieve the current thread's TCB from the queue
    public TCB getMyTcb( ) {
        Thread myThread = Thread.currentThread( ); // Get my thread object
        synchronized( queue ) {
            for ( int level = 0; level < 3; level++ ) {
                for ( int i = 0; i < queue[level].size( ); i++ ) {
                    TCB tcb=queue[level].elementAt( i );
                    Thread thread = tcb.getThread( );
                    if ( thread == myThread ) // if this is my TCB, return it
                        return tcb;
                }
            }
        }
        return null;
    }

    // A new feature added to the original algorithm 
    // Return the maximal number of threads to be spawned in the system
    public int getMaxThreads( ) {
        return tids.length;
    }

    public Scheduler( ) {
        timeSlice = DEFAULT_TIME_SLICE;
        initTid( DEFAULT_MAX_THREADS );
        for ( int i = 0; i < 3; i++ ) queue[i] = new Vector<TCB>( );
    }

    public Scheduler( int quantum ) {
        timeSlice = quantum;
        initTid( DEFAULT_MAX_THREADS );
        for ( int i = 0; i < 3; i++ ) queue[i] = new Vector<TCB>( );
    }

    // A new feature added to the original algorithm 
    // A constructor to receive the max number of threads to be spawned
    public Scheduler( int quantum, int maxThreads ) {
        timeSlice = quantum;
        initTid( maxThreads );
        for ( int i = 0; i < 3; i++ ) queue[i] = new Vector<TCB>( );
    }

    private void schedulerSleep( ) {
        try {
            Thread.sleep( timeSlice / 2 );
        } catch ( InterruptedException e ) {
        }
    }

    // A modified addThread of the original algorithm
    public TCB addThread( Thread t ) {
        TCB parentTcb = getMyTcb( ); // get my TCB and find my TID
        int pid = ( parentTcb != null ) ? parentTcb.getTid( ) : -1;
        int tid = getNewTid( ); // get a new TID
        if ( tid == -1)
            return null;
        TCB tcb = new TCB( t, tid, pid ); // create a new TCB
        queue[0].add( tcb );
        return tcb;
    }

    // A new feature added to the original algorithm
    // Removing the TCB of a terminating thread
    public boolean deleteThread( ) {
        TCB tcb = getMyTcb( );
        if ( tcb!= null ) {
            this.interrupt( );
            return tcb.setTerminated( );
        } else
            return false;
    }

    public void sleepThread( int milliseconds ) {
        try {
            sleep( milliseconds );
        } catch ( InterruptedException e ) { }
    }
    
    // A modified run of the original algorithm
    public void run( ) {
        // the current running thread
        Thread current = null;
        // TCB with data about the current thread
        TCB currentTCB = null;
        // The last running threads TCB
        TCB prevTCB = null;
        // TODO: used as a flag to continue previous thread?
        int slice[] = new int[3];

        // start out with no queues in each lvl
        for ( int i = 0; i < 3; i++ )
            slice[i] = 0;

        // represents the forever running scheduler loop
        while ( true ) {
            try {
                // starting at first queue
                int level = 0;

                // loop over each queue
                for ( ; level < 3; level++ ) {
                    // TODO: why do we need this?
                    if ( slice[level] == 0 ) {
                        // if there's nothing in the queue
                        if ( queue[level].size( ) == 0 )
                            // loop again to check the next queue up
                            continue;
                        // this queue is not empty, grab element
                        currentTCB = queue[level].firstElement( );

                        // we have our thread, stop looping
                        break;
                    }
                    else {
                        // keep processing the previous thread
                        currentTCB = prevTCB;
                        break;
                    }
                }

                // we looped through without finding a thread
                if ( level == 3 )
                    continue;

                // hw2b: TODO: implement the code in this if statement
                if ( currentTCB.getTerminated( ) == true ) {
                    // Remove this thread from queue[level]
                    queue[level].remove( currentTCB );
                    // Return this thread id
                    returnTid( currentTCB.getTid() );
                    SysLib.cerr(slice[level]+"\n");
                    // slice[level] must be 0
                    slice[level] = 0;
                    continue;
                }
                current = currentTCB.getThread( );

                // hw2b: TODO: implement the code based on the following comment
                if ( ( current != null ) ) {
                    // If current is alive, resume it otherwise start it.
                    // The same logic as Scheduler_rr.java
                    // Just copy the logic here.
                    if ( current.isAlive( ) )
                        current.resume( );
                    else
                        // Spawn must be controlled by Scheduler
                        // Scheduler must start a new thread
                        current.start( );
                }

                // hw2b: TODO: implement the code based on the following comment
                // Scheduler should sleep here.
                schedulerSleep( );
                // If current is alive, suspend it.
                // The same logic as Scheduler_rr.java
                // Just copy the logic here

                prevTCB = currentTCB;
                synchronized( queue[level] ) {
                    if ( current != null && current.isAlive( ) )
                        current.suspend( );

                    if ( level == 0 ){
                        queue[0].remove( currentTCB );
                        queue[1].add( currentTCB );
                    } else if ( level == 1) {
                        queue[1].remove( currentTCB );
                        queue[2].add( currentTCB );
                    } else  if ( level ==2 ){
                        queue[2].remove( currentTCB );
                        queue[2].add( currentTCB );
                    }
                }
                // hw2b: TODO: implement the code based on the following comment
                // This is the heart of Prog2B!!!!
                // Update slice[level].
                slice[level]++;
                // if slice[level] returns to 0,
                //   currentThread must go to the next level or
                //   rotate back in queue[2]
            } catch ( NullPointerException e3 ) { };
        }
    }
}
