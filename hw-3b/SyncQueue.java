/*
 * This is a template of SyncQueue.java. Chagne this file name into SyncQueue.java and
 * complete the implementation
 */
public class SyncQueue {
    
    // don't add any new data members
    private QueueNode queue[] = null;
    private final int COND_MAX = 10;
    private final int NO_TID = -1;

	// don't change this private function
    private void initQueue( int condMax ) {
		queue = new QueueNode[ condMax ];
		for ( int i = 0; i < condMax; i++ )
			queue[i] = new QueueNode( );
	}

    public SyncQueue( ) {
        // TODO: implement this constructor
        // Assume SyncQueue( COND_MAX );
        initQueue(COND_MAX);
    }

    public SyncQueue( int condMax ) {
        // TODO: implement this constructor
        initQueue(condMax);
    }

    int enqueueAndSleep( int condition ) {
        // TODO: implement this method
        // Verify the correctness of condition.
        if (condition > -1 && condition < queue.length)
            // Call the corresponding queue[ ].sleep( ).
            // Return the corresponding child thread ID.
            return queue[condition].sleep();
        return 0;
    }

    void dequeueAndWakeup( int condition, int tid ) {
        // TODO: implement this method
        // verify the correctness of condition.
        if (condition > -1 && condition < queue.length)
            // Call the corresponding queue[ ].wakeup( ... );
            queue[condition].wakeup(tid);
    }

    void dequeueAndWakeup( int condition ) {
        // TODO: implement this method
        if (condition > -1 && condition < queue.length)
            // Assume tid = 0.
            queue[condition].wakeup(0);
    }
}
