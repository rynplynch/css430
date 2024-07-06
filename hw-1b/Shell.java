import java.io.*;
import java.util.*;

/**
 * Is a ThreadOS Shell interpreter.
 */
public class Shell extends Thread
{
    /**
     * Is a default constructor.
     */
    public Shell( ) {
    }

    /**
     * Is the Shell.java thread's main body. It repeatedly prints a
     * command prompt "shell[number]% ", reads a line of user
     * commands, interprets each command to launch the corresponding
     * user thread, and check with a command delimitor "&amp;" or ";"
     * whether it should wait for the termination or go onto the next
     * command interpretation.
     */
    public void run( ) {
        // create new variable which tracks current line on cli
        for ( int line = 1; ; line++ ) {
            // commands found on current line
            String cmdLine = "";

            do {
                // ready new string buffer for user input
                StringBuffer inputBuf = new StringBuffer( );

                // print the current cli line for the user
                SysLib.cerr( "shell[" + line + "]% " );

                // interrupt thread, receive input from the user
                SysLib.cin( inputBuf );
                // continue thread execution after release

                // convert user input to string and store in command string
                cmdLine = inputBuf.toString( );
            }
            // if input buffer had to characters, prompt for input again
            while ( cmdLine.length( ) == 0 );

            // converts input from user into individual strings
            String[] args = SysLib.stringToArgs( cmdLine );

            // tracks the first argument of the current command
            int first = 0;

            // loop through each argument
            for ( int i = 0; i < args.length; i++ ) {
                // if current argument equals ';' or '&' or there remain no more arguments
                if ( args[i].equals( ";" ) || args[i].equals( "&" )
                     || i == args.length - 1 ) {
                    // index of the last argument in command
                    int last = ( i==args.length - 1 ) ? i+1 : i;

                    // pull new command from string of args
                    String[] command = generateCmd( args, first, last);

                    // as long as the command is not null, process it
                    if ( command != null ) {
                        // the first item should always be the name of a command
                        String cmd = command[0];

                        // if the command is exit
                        if (cmd.equals("exit")){
                            // tells ThreadOS to make exit system call
                            SysLib.exit();

                            // return from Shells run method;
                            return;
                        }
                        // pass command off to ThreadOS to handle
                        int threadID = SysLib.exec(command);

                        // if the delimiter equals ;
                        if (args[i].equals(";")){
                            // wait for a process to finish
                            int finishedID = SysLib.join();

                            // while the returned processor ID doesn't match the one we just created
                            while (finishedID != threadID)
                                // returned threat was not the one we wanted, wait for another
                                finishedID = SysLib.join();
                        }
                        // check if thread creation was successful
                        if( threadID < 0)
                            SysLib.cerr("Command execution failed: " + cmd + "\n");
                    }
                    // move first index to point at argument of next command
                    first = i + 1;
                }
            }
        }
    }

    /**
     * Creates a new command using a list of string arguments
     * args: the complete list of arguments
     * first: index for the first argument of the command
     * last: index for the last argument of the command
     */
    private String[] generateCmd( String args[], int first, int last ) {
        // if the last argument is ';' or '&'
        if ( (args[last-1].equals(";")) || (args[last-1].equals("&")) )
            // ignore it while processing
            last = last -1;

        // if there is no argument left to process
        if ( last - first <= 0 )
            return null;

        // create a new command with length of the remaining arguments
        String[] command = new String[ last - first ];

        // loop over every remaining argument
        for ( int i = first ; i < last; i++ )
            // add each argument to the returned command
            command[i - first] = args[i];

        return command;
    }
}
