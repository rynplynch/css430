#include <sys/types.h>   // for fork, wait
#include <sys/wait.h>    // for wait
#include <unistd.h>      // for fork, pipe, dup, close
#include <stdio.h>       // for NULL, perror
#include <stdlib.h>      // for exit

#include <iostream>      // for cout

using namespace std;

int main( int argc, char** argv ) {
  int fds[2][2];
  int pid;

  if ( argc != 2 ) {
    cerr << "Usage: processes command" << endl;
    exit( -1 );
  }

  // fork a child
  if ( ( pid = fork( ) ) < 0 ) {
    perror( "fork error" );
  }
  else if ( pid == 0 ) {
    // I'm a child

	// TODO: You only need to implement your code in this "else if" block
	// TIP: DO check if system call succeed or not; DO use exit() properly
    // create a pipe using fds[0]
    // fork a grandchild
    // if I'm a grandchild
	// create a pipe using fds[1]
	// fork a great grandchild
 	// if I'm a great grandchild
        // execute "ps"
    // else if I'm a grandchild
        // execute "grep"
    // else if I'm a child
		// execute "wc"
  }
  else {
    // I'm a parent
    wait( NULL );
    cout << "commands completed" << endl;
  }
}
