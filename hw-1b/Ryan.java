import java.io.*;
import java.util.*;

public class Ryan extends Thread
{
    private String message;

    public Ryan() {
        message = "No arguments provided";
    }

    public Ryan( String[] args ) {
        message = args[0];
    }

    public void run( ) {
        SysLib.cout(message + "\n");
        SysLib.exit();
    }
}
