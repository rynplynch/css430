#include <alloca.h>
#include <complex>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <ios>
#include <iterator>
#include <stdio.h>  // for NULL, perror
#include <stdlib.h> // for exit
#include <string>
#include <sys/types.h> // for fork, wait
#include <sys/wait.h>  // for wait
#include <unistd.h>    // for fork, pipe, dup, close

#include <iostream> // for cout

int main(int argc, char *argv[]) {
  fork();
  pid_t pid = fork();

  if (pid == 0)
    fork();
  wait(NULL);
  printf("PID: %d", getpid());
  return 0;
}
