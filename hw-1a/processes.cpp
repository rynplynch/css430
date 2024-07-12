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

  // childID will be unassigned in child process
  if (childID == 0) {
    // create new process while checking for error
    if ((grandID = fork()) < 0)
      // error occurred
      perror("Failed to create ps process");

    // grandID will be unassigned in grandchild process
    if (grandID == 0) {
      // write to the output of the grep fd
      dup2(fdToGrep[1], STDOUT_FILENO);

      // close fds we no longer need
      close(fdToGrep[0]);
      close(fdToGrep[1]);
      close(fdToWc[0]);
      close(fdToWc[1]);

      // execute ps in the grandchild process
      execlp("ps", "ps", "-A", NULL);
    }

    // accept input direct toward grep
    dup2(fdToGrep[0], STDIN_FILENO);

    // close not needed fs
    close(fdToGrep[0]);
    close(fdToGrep[1]);

    // direct output toward wc
    dup2(fdToWc[1], STDOUT_FILENO);

    // close not needed fs
    close(fdToWc[0]);
    close(fdToWc[1]);

    // wait for the ps command to execute
    waitpid(grandID, NULL, 0);

    // call grep using the $PATH enviroment variable
    // pass in the -i flag to ignore casing
    // pass in user provided argument to search for
    execlp("grep", "grep", "-i", argv[1], NULL);
  }

  // this process only needs output from wc
  close(fdToGrep[0]);
  close(fdToGrep[1]);

  // read from the output of grep
  dup2(fdToWc[0], STDIN_FILENO);

  // also close these
  close(fdToWc[0]);
  close(fdToWc[1]);

  // make sure all child process are done
  wait(NULL);

  // count the number of lines output from grep
  execlp("wc", "wc", "-l", NULL);

  return 0;
}
