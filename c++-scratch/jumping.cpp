#include <csetjmp>
#include <cstdio>

using namespace std;

// variable for storing an enviroment
jmp_buf env;

void second() {
  printf("second\n");
  longjmp(env, 1);
}

void first() {
  second();
  printf("called first\n");
}

int main(int argc, char *argv[]) {
  if (!setjmp(env))
    first();
  else
    printf("returned to main\n");

  return 0;
}
