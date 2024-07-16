#include <iostream> // cout
#include <queue>    // queue
#include <setjmp.h> // setjmp( )
#include <signal.h> // signal( )
#include <stdio.h>  // perror( )
#include <stdlib.h> // exit( )
#include <string.h> // memcpy
#include <unistd.h> // sleep( ), alarm( )

#define scheduler_init()                                                       \
  {                                                                            \
    if (setjmp(main_env) == 0)                                                 \
      scheduler();                                                             \
  }

#define scheduler_start()                                                      \
  {                                                                            \
    if (setjmp(main_env) == 0)                                                 \
      longjmp(scheduler_env, 1);                                               \
  }

// save threads current function call variables
#define capture()                                                              \
  {                                                                            \
    /* assembly directives to grab top and bottom pointer of stack*/           \
    register void *sp asm("sp");                                               \
    register void *bp asm("bp");                                               \
                                                                               \
    /* calculate the size of the functions stack and save in tcb */            \
    cur_tcb->size = (int)((long long int)bp - (long long int)sp);              \
                                                                               \
    /* save stack pointer */                                                   \
    cur_tcb->sp = sp;                                                          \
                                                                               \
    /* reserve memory location for stack */                                    \
    cur_tcb->stack = malloc(cur_tcb->size);                                    \
                                                                               \
    /* use memory copy to save stack into threads control block*/              \
    memcpy(cur_tcb->stack, sp, cur_tcb->size);                                 \
                                                                               \
    /* add the threads tcb to the queue of active process */                   \
    thr_queue.push(cur_tcb);                                                   \
  }

// check if the current thread needs to context switch back to scheduler
#define sthread_yield()                                                        \
  {                                                                            \
    /* check if alarmed flag set to true by signal handler */                  \
    if (alarmed) {                                                             \
      /* lower flag so we don't yield on next check */                         \
      alarmed = false;                                                         \
                                                                               \
      /* save current environment into threads control block */                \
      if (setjmp(cur_tcb->env) == 0) {                                         \
        /* also save function parameters into thread control block */          \
        capture();                                                             \
                                                                               \
        /* move over to scheduler environment to switch to next thread */      \
        longjmp(scheduler_env, 1);                                             \
      }                                                                        \
      /* execution handed back to thread, restore function parameters */       \
      memcpy(cur_tcb->sp, cur_tcb->stack, cur_tcb->size);                      \
    }                                                                          \
  }

#define sthread_init()                                                         \
  {                                                                            \
    if (setjmp(cur_tcb->env) == 0) {                                           \
      capture();                                                               \
      longjmp(main_env, 1);                                                    \
    }                                                                          \
    memcpy(cur_tcb->sp, cur_tcb->stack, cur_tcb->size);                        \
  }

#define sthread_create(function, arguments)                                    \
  {                                                                            \
    if (setjmp(main_env) == 0) {                                               \
      func = &function;                                                        \
      args = arguments;                                                        \
      thread_created = true;                                                   \
      cur_tcb = new TCB();                                                     \
      longjmp(scheduler_env, 1);                                               \
    }                                                                          \
  }

#define sthread_exit()                                                         \
  {                                                                            \
    if (cur_tcb->stack != NULL)                                                \
      free(cur_tcb->stack);                                                    \
    longjmp(scheduler_env, 1);                                                 \
  }

using namespace std;

static jmp_buf main_env;
static jmp_buf scheduler_env;

// Thread control block
class TCB {
public:
  TCB() : sp(NULL), stack(NULL), size(0) {}
  jmp_buf env; // the execution environment captured by set_jmp( )
  void *sp;    // the stack pointer
  void *stack; // the temporary space to maintain the latest stack contents
  int size;    // the size of the stack contents
};
static TCB *cur_tcb; // the TCB of the current thread in execution

// The queue of active threads
static queue<TCB *> thr_queue;

// Alarm caught to switch to the next thread
static bool alarmed = false;
static void sig_alarm(int signo) { alarmed = true; }

// A function to be executed by a thread
void (*func)(void *);
void *args = NULL;
static bool thread_created = false;

static void scheduler() {
  // save the environment for the scheduler
  if (setjmp(scheduler_env) == 0) {
    cerr << "scheduler: initialized" << endl;

    // set the function that gets called by the alarm signal
    if (signal(SIGALRM, sig_alarm) == SIG_ERR) {
      // notify user of error with signal handler
      perror("signal function");

      // if we fail to set alarm function exit with error
      exit(-1);
    }

    // return to setjmp in scheduler_init
    longjmp(main_env, 1);
  }
  /**
   * every time longjmp is called with the scheduler_env we are brought here
   */

  // check if it was called from sthread_create( )
  if (thread_created == true) {
    // lower created flag for other calls to scheduler environment
    thread_created = false;

    // call the created threads function and pass in arguments as well
    (*func)(args);
  }

  // grab the thread control block in front of queue
  if ((cur_tcb = thr_queue.front()) != NULL) {
    // remove tcb from front of queue
    thr_queue.pop();

    // allocate a time quantum of 5 seconds
    alarm(5);

    // go back to the execution of threads function
    longjmp(cur_tcb->env, 1);
  }

  // no threads to schedule, simply return
  cerr << "scheduler: no more threads to schedule" << endl;
  longjmp(main_env, 2);
}
