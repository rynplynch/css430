#include <cstddef>
#include <iostream>  // cout
#include <pthread.h> // pthread
#include <stdio.h>
#include <stdlib.h>   // atoi( )
#include <sys/time.h> // gettimeofday
#include <unistd.h>   // sleep( )

#define PHILOSOPHERS 5
#define MEALS 3

using namespace std;

class Table2 {
public:
  Table2() {
    // HW3A TODO: initialize the table
    // initialize the mutex, conditional varilabels and state
  }
  void pickup(int i) {
    // HW3A TODO: implement the pickup method by referring to the textbook
    // Figure 5.18.
  }

  void putdown(int i) {
    // HW3A TODO: implement the putdown method by referring to the textbook
    // Figure 5.18.
    cout << "philosopher[" << i << "] put down chopsticks" << endl;
  }

private:
  // private data members
  enum { THINKING, HUNGRY, EATING } state[PHILOSOPHERS];
  pthread_mutex_t lock;
  pthread_cond_t self[PHILOSOPHERS];

  // private method, called in pickup() and putdown()
  void test(int i) {
    // HW3A TODO: implement by yourself by referring to the textbook
    // Figure 5.18.
  }
};

class Table1 {
public:
  Table1() {
    // HW3A TODO: initialize the mutex lock (tip: one line of code)
    pthread_mutex_init(&lock, NULL);
  }
  void pickup(int i) {
    // HW3A TODO: lock the mutex (tip: one line of code)
    pthread_mutex_lock(&lock);
    cout << "philosopher[" << i << "] picked up chopsticks" << endl;
  }

  void putdown(int i) {
    cout << "philosopher[" << i << "] put down chopsticks" << endl;
    // HW3A TODO: unlock the mutex (tip: one line of code)
    pthread_mutex_unlock(&lock);
  }

private:
  // HW3A TODO: define a mutex lock (tip: one line of code)
  pthread_mutex_t lock;
};

class Table0 {
public:
  void pickup(int i) {
    cout << "philosopher[" << i << "] picked up chopsticks" << endl;
  }

  void putdown(int i) {
    cout << "philosopher[" << i << "] put down chopsticks" << endl;
  }
};

static Table2 table2;
static Table1 table1;
static Table0 table0;

static int table_id = 0;

void *philosopher(void *arg) {
  int id = *(int *)arg;

  for (int i = 0; i < MEALS; i++) {
    switch (table_id) {
    case 0:
      table0.pickup(id);
      sleep(1);
      table0.putdown(id);
      break;
    case 1:
      table1.pickup(id);
      sleep(1);
      table1.putdown(id);
      break;
    case 2:
      table2.pickup(id);
      sleep(1);
      table2.putdown(id);
      break;
    }
  }
  return NULL;
}

int main(int argc, char **argv) {
  pthread_t threads[PHILOSOPHERS];
  pthread_attr_t attr;
  int id[PHILOSOPHERS];
  table_id = atoi(argv[1]);

  pthread_attr_init(&attr);

  struct timeval start_time, end_time;
  gettimeofday(&start_time, NULL);
  for (int i = 0; i < PHILOSOPHERS; i++) {
    id[i] = i;
    pthread_create(&threads[i], &attr, philosopher, (void *)&id[i]);
  }

  for (int i = 0; i < PHILOSOPHERS; i++)
    pthread_join(threads[i], NULL);
  gettimeofday(&end_time, NULL);

  sleep(1);
  cout << "time = "
       << (end_time.tv_sec - start_time.tv_sec) * 1000000 +
              (end_time.tv_usec - start_time.tv_usec)
       << endl;

  return 0;
}
