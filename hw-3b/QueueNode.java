/*
 * This is a template of QueueNode.java. Change this file name into QueueNode.java
 * and implement the logic.
 */

import java.util.*;

public class QueueNode {
    private Vector<Integer> tidQueue; // maintains a list of child TIDs who called wakeup( ).

    public QueueNode( ) {
        // TODO: implement this constructor
        tidQueue = new Vector<Integer>();
    }

    public synchronized int sleep( ) {
        // TODO: implement this method
        // If tidQueue has nothing, call wait( ).
        if (tidQueue.isEmpty()){
            try{
                wait();
            }
            catch (InterruptedException e){

            }
        }
        // Otherwise, get one child TID from tidQueue.
            // return it.
            return tidQueue.remove(0);

    }

    public synchronized void wakeup( int tid ) {
        // TODO: implement this method
        // Add this child TID to tidQueue.
        tidQueue.addElement(tid);
        // Notify the parent.
        notify();
    }
}
