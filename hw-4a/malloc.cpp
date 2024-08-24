#include <iomanip>
#include <iostream> // cout
#include <limits.h> // LONG_MAX
#include <stdio.h>  // printf( )
#include <unistd.h> // sbrk( )
using namespace std;

static bool initialized = false;
static void *heap_top; // the beginning of the heap space
static void *
    heap_end; // the current boundary of the heap space, obtained from sbrk( 0 )

class MCB { // memory control block
public:
  // true(1): this memory partition is available
  // false(0): this memory partition is unavailable.
  int available;

  // MCB size + the user data size
  int size;
};

void free_(void *dealloc_space) {
  MCB *mcb;

  // TODO: Task 4: implement by yourself (could be implemented just in one
  // line). locate this partition's mcb address from dealloc_space deallocated
  // space follows its mcb;

  // calculate the MCB address given the address of already allocated space
  long long int intAddress = (long long int)dealloc_space - sizeof(MCB);

  // cast that number into an address that points to a MCB
  mcb = (MCB *)(intAddress);

  cout << "MCB at 0x" << hex << intAddress << " now available!" << endl;
  cout << "It has a size of 0x" << hex << mcb->size << endl << endl;
  mcb->available = true;
  return;
}

void *malloc_f(long size) {
  struct MCB *cur_mcb; // current MCB
  void *new_space =
      NULL; // this is a pointer to a new memory space allocated for a user

  if (!initialized) {
    // find the end of heap memory, upon an initialization
    heap_end = sbrk(0);
    heap_top = heap_end;
    initialized = true;
  }
  printf("The end of the heap was here: %p\n", heap_end);
  // append an MCB in front of a requested memory space
  size = size + sizeof(MCB);

  // TODO: Task 1: implement by yourself (could be implemented through 15 lines
  // of code). scan each mcb from the top to the bottom of the heap let cur_mcb
  // point to each mcb you are scanning if cur_mcb->available and cur_mcb->size
  // fits size, new_space points to this mcb

  // the first MCB we look at is at the top of the heap
  cur_mcb = (MCB *)heap_top;

  // if the current address does not equal the end of heap address
  while (cur_mcb != heap_end) {
    // get the address of the next MCB already on the heap
    long long int nextMCB =
        (((long long int)cur_mcb) + ((long long int)cur_mcb->size));

    // check if the current MCB can accept the User data
    if (cur_mcb->available && cur_mcb->size >= size) {
      // get the address of the MCB that holds the left over space
      long long int newMCB = (((long long int)cur_mcb) + ((long long int)size));

      printf("found an MCB that can be used at: %p\n", cur_mcb);
      cout << "Space taken by this MCB: 0x" << hex << cur_mcb->size << endl;
      // set this MCB as taken
      cur_mcb->available = false;

      // save this MCB as the return address
      new_space = cur_mcb;

      // if the new MCB moves into the address space of the next MCB
      if ((newMCB + 8) >= nextMCB) {
        // don't create a new MCB and keep the original size of the current MCB
        break;
      } else {
        // update this MCB with the size of the new User data
        cur_mcb->size = size;

        // point to the newly create MCB
        cur_mcb = (MCB *)newMCB;

        // set it as available
        cur_mcb->available = true;

        // set its size as the left over space
        cur_mcb->size = nextMCB - newMCB;
        cout << "space of new MCB: 0x" << hex << cur_mcb->size << endl;
        break;
      }
      // current MCB can not accept User Data
    } else {
      // move to the next MCB in the stack
      cur_mcb = (MCB *)nextMCB;
    }
  }

  // no space found yet
  if (new_space == NULL) {
    // TODO: Task 2: implement by yourself (could be implemented through 10
    // lines of code).
    // get a space from OS
    // old boundary now becomes new_space, i.e., initialize new_space with
    // heap_end heap_end will go down by size initialize cur_mcb with new_space
    // and size.
    new_space = heap_end;

    // system call to allocate space for another MCB and its data
    sbrk(size);

    // new address for the end of the heap
    heap_end = sbrk(0);

    // set the MCB as unavailable
    cur_mcb->available = false;

    // save the size of the user data
    cur_mcb->size = size;
  }
  // new space is after new MCB
  return (void *)((long long int)new_space + sizeof(MCB));
}

void *malloc_b(long size) {
  struct MCB *cur_mcb; // current MCB
  void *new_space =
      NULL; // this is a pointer to a new memory space allocated for a user

  if (!initialized) {
    // find the end of heap memory, upon an initialization
    heap_end = sbrk(0);
    heap_top = heap_end;
    initialized = true;
  }

  // append an MCB in front of a requested memroy space
  size = size + sizeof(MCB);

  // TODO: Task 3: implement by yourself (could be implemented in through 20
  // lines of code). scan each mcb from the top to the bottom of the heap let
  // cur_mcb point to each mcb you are scanning if cur_mcb->available and
  // cur_mcb->size fits size and cur_mcb->size is the best size so far
  //   temporarily memorize this best size so far and this best mcb so far
  // After scan, check the best mcb so far. If it is not null
  //   new_space points to this best mcb so rar
  // the first MCB we look at is at the top of the heap
  cur_mcb = (MCB *)heap_top;

  // the current best fit MCB
  MCB *best_mcb = NULL;
  // if the current address does not equal the end of heap address
  while (cur_mcb != heap_end) {
    // get the address of the next MCB already on the heap
    long long int nextMCB =
        (((long long int)cur_mcb) + ((long long int)cur_mcb->size));

    // check if the current MCB can accept the User data
    if (cur_mcb->available && cur_mcb->size >= size) {
      // compare this usable MCB with the best MCB so far
      if (best_mcb == NULL || best_mcb->size > cur_mcb->size) {
        best_mcb = cur_mcb;
      }
    }
    cur_mcb = (MCB *)nextMCB;
  }

  // we found a space in the heap to use
  if (best_mcb != NULL) {
    new_space = best_mcb;

    cur_mcb = best_mcb;

    long long int nextMCB =
        (((long long int)cur_mcb) + ((long long int)cur_mcb->size));

    // get the address of the MCB that holds the left over space
    long long int newMCB = (((long long int)cur_mcb) + ((long long int)size));

    printf("found an MCB that can be used at: %p\n", cur_mcb);
    cout << "Space taken by this MCB: 0x" << hex << cur_mcb->size << endl;
    // set this MCB as taken
    cur_mcb->available = false;

    // save this MCB as the return address
    new_space = cur_mcb;

    // if the new MCB moves into the address space of the next MCB
    if ((newMCB + 8) >= nextMCB) {
      // don't create a new MCB and keep the original size of the current MCB
    } else {
      // update this MCB with the size of the new User data
      cur_mcb->size = size;

      // point to the newly create MCB
      cur_mcb = (MCB *)newMCB;

      // set it as available
      cur_mcb->available = true;

      // set its size as the left over space
      cur_mcb->size = nextMCB - newMCB;
      cout << "space of new MCB: 0x" << hex << cur_mcb->size << endl;
    }
  }

  // no space found yet
  if (new_space == NULL) {
    // TODO: Task 3: Just copy and past the logic from malloc_f to here.
    // The same code as Task 2
    new_space = heap_end;

    // system call to allocate space for another MCB and its data
    sbrk(size);

    // new address for the end of the heap
    heap_end = sbrk(0);

    // set the MCB as unavailable
    cur_mcb->available = false;

    // save the size of the user data
    cur_mcb->size = size;
  }

  // new space is after new MCB
  return (void *)((long long int)new_space + sizeof(MCB));
}
