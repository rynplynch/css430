#include <unistd.h> // sbrk( )
#include <limits.h> // LONG_MAX

using namespace std;

static bool initialized = false;
static void *heap_top; // the beginning of the heap space
static void *heap_end; // the current boundary of the heap space, obtained from sbrk( 0 )

class MCB { // memory control block
public:
  int available; // true(1): this memory partition is available, false(0) unavailalbe.
  int size;      // MCB size + the user data size
};

void free_( void *dealloc_space ) {
  MCB *mcb;

  // TODO: Task 4: implement by yourself (could be implemented just in one line).
  // locate this partition's mcb address from dealloc_space
  // deallocated space follows its mcb;

  mcb->available = true;
  return;
}

void *malloc_f( long size ) {
  struct MCB *cur_mcb;          // current MCB
  void *new_space = NULL; // this is a pointer to a new memory space allocated for a user

  if( !initialized )   {
    // find the end of heap memory, upon an initialization
    heap_end = sbrk( 0 );
    heap_top = heap_end;
    initialized = true;
  }

  // append an MCB in front of a requested memroy space
  size = size + sizeof( MCB );

  // TODO: Task 1: implement by yourself (could be implemented through 15 lines of code).
  // scan each mcb from the top to the bottom of the heap
  // let cur_mcb point to each mcb you are scanning
  // if cur_mcb->available and cur_mcb->size fits size, new_space points to this mcb

  // no space found yet
  if ( new_space == NULL ) {
  	// TODO: Task 2: implement by yourself (could be implemented through 10 lines of code).
    // get a space from OS
    // old boundary now becomes new_space, i.e., initialize new_space with heap_end
    // heap_end will go down by size
    // initialize cur_mcb with new_space and size.
  }

  // new space is after new MCB
  return (void *)( ( long long int )new_space + sizeof( MCB ) );
}

void *malloc_b( long size ) {
  struct MCB *cur_mcb;          // current MCB
  void *new_space = NULL; // this is a pointer to a new memory space allocated for a user

  if( !initialized )   {
    // find the end of heap memory, upon an initialization
    heap_end = sbrk( 0 );
    heap_top = heap_end;
    initialized = true;
  }

  // append an MCB in front of a requested memroy space
  size = size + sizeof( MCB );

  // TODO: Task 3: implement by yourself (could be implemented in through 20 lines of code).
  // scan each mcb from the top to the bottom of the heap
  // let cur_mcb point to each mcb you are scanning
  // if cur_mcb->available and cur_mcb->size fits size and cur_mcb->size is the best size so far
  //   temporarily memorize this best size so far and this best mcb so far
  // After scan, check the best mcb so far. If it is not null
  //   new_space points to this best mcb so rar
  

  // no space found yet
  if ( new_space == NULL ) {
    // TODO: Task 3: Just copy and past the logic from malloc_f to here.  	
    // The same code as Task 2
  }

  // new space is after new MCB
  return (void *)( ( long long int )new_space + sizeof( MCB ) );
}
