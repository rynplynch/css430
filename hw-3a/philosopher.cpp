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
    pthread_mutex_init(&lock, NULL);
    for (int i = 0; i < PHILOSOPHERS; i++) {
      pthread_cond_init(&self[i], NULL);
      state[i] = THINKING;
    }
  }
  void pickup(int i) {
    // HW3A TODO: implement the pickup method by referring to the textbook
    // Figure 5.18.
    // set the state of the philosopher to hungry
    pthread_mutex_lock(&lock);
    state[i] = HUNGRY;

    // check if philosopher can pick up both chopsticks
    test(i);

    // as long as the philosopher is not already eating
    if (state[i] != EATING) {
      // wait until the philosopher can pick up both chopsticks
      pthread_cond_wait(&self[i], &lock);

      // pick up the chopsticks
      cout << "philosopher[" << i << "] picked up chopsticks" << endl;
    }
    pthread_mutex_unlock(&lock);
  }

  void putdown(int i) {
    // HW3A TODO: implement the putdown method by referring to the textbook
    // Figure 5.18.
    // Yummy! All done eating, go back to thinking
    pthread_mutex_lock(&lock);
    state[i] = THINKING;

    // grab the index of the neighboring philosophers
    int l = (i + (PHILOSOPHERS - 1)) % PHILOSOPHERS;
    int r = (i + 1) % PHILOSOPHERS;

    // let our neighbors know that they can use our chopsticks now (GROSS)
    test(l);
    test(r);

    cout << "philosopher[" << i << "] put down chopsticks" << endl;
    pthread_mutex_unlock(&lock);
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
    // grab the index of the neighboring philosophers
    int l = (i + (PHILOSOPHERS - 1)) % PHILOSOPHERS;
    int r = (i + 1) % PHILOSOPHERS;

    // cout << "index of thinker: " << i << endl;
    // cout << "index of left neighbor: " << l << endl;
    // cout << "index of right neighbor: " << r << endl;

    // if the current philosopher is hungry
    if ((state[i] == HUNGRY) &&
        // and the philosopher to the left is not eating
        (state[l] != EATING) &&
        // and the philosopher to the right is not eating
        (state[r] != EATING)) {
      // the current philosopher can eat
      state[i] = EATING;

      // let waiting process in queue know they can go ahead and eat
      pthread_cond_signal(&self[i]);
    }
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
