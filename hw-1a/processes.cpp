#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <stdio.h>     // for NULL, perror
#include <stdlib.h>    // for exit
#include <sys/types.h> // for fork, wait
#include <sys/wait.h>  // for wait
#include <unistd.h>    // for fork, pipe, dup, close

#include <iostream> // for cout

using namespace std;

int main(int argc, char **argv) {
  // the program only accepts 1 argument from cli
  // the first arg is always the program name
  if (argc != 2) {
    cerr << "Usage: processes command" << endl;
    exit(-1);
  }

  // pipe that goes to grep
  int fdToGrep[2];

  // pipe that goes to wc
  int fdToWc[2];

  // open our pipes
  if (pipe(fdToGrep) == -1 || pipe(fdToWc) == -1)
    perror("Pipe failed to open");

  // get processor ID of the caller, so this process
  int parnetID = getpid();

  // to be set when forking
  int childID;
  int grandID;

  // create new process while checking for error
  if ((childID = fork()) < 0)
    // error occurred
    perror("Failed to create grep process");

  if (childID == 0) {
    dup2(fdToGrep[1], STDOUT_FILENO);
    close(fdToGrep[0]);
    close(fdToGrep[1]);

    execlp("ps", "ps", "-A", NULL);
  }

  grandID = fork();
    // create new process while checking for error
    if ((grandID = fork()) < 0)
      // error occurred
      perror("Failed to create ps process");


  if (grandID == 0) {
    // accept input direct toward grep
    dup2(fdToGrep[0], STDIN_FILENO);

    // close cloned fds
    close(fdToGrep[0]);
    close(fdToGrep[1]);

    // direct output toward wc
    dup2(fdToWc[1], STDOUT_FILENO);

    // close cloned fs
    close(fdToWc[0]);
    close(fdToWc[1]);

    execlp("grep", "grep", "-i", argv[1], NULL);
  }

  // it's critical that all fds are closed
  // once all writers are closed grep knows to stop reading
  close(fdToGrep[0]);
  close(fdToGrep[1]);

  // read from the output of grep
  dup2(fdToWc[0], STDIN_FILENO);

  // also close these
  close(fdToWc[0]);
  close(fdToWc[1]);

  waitpid(childID, NULL, 0);
  waitpid(grandID, NULL, 0);

  execlp("wc", "wc", "-l", NULL);

  return 0;
}
