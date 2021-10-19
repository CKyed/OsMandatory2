#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "mymem.h"
#include <time.h>


/* The main structure for implementing memory allocation.
 * You may change this to fit your implementation.
 */

struct memoryList //This is a node
{
    // doubly-linked list
    struct memoryList *last;
    struct memoryList *next;

    size_t size;         // How many bytes in this block?
    char alloc;          // 1 if this block is allocated,
    // 0 if this block is free.
    void *ptr;           // location of block in memory pool.
};

void *malloc_first(size_t requested);
void printNode(struct memoryList *node);
void removeNode(struct memoryList *node);
void mergeFreeNodes(struct memoryList *firstNode, struct memoryList *lastNode);
void freeNode(struct memoryList *node);


strategies myStrategy = NotSet;    // Current strategy


size_t mySize;
void *myMemory = NULL;

static struct memoryList *head;
static struct memoryList *lastVisited; //Only used for next fit strategy.


/* initmem must be called prior to mymalloc and myfree.

   initmem may be called more than once in a given exeuction;
   when this occurs, all memory you previously malloc'ed  *must* be freed,
   including any existing bookkeeping data.

   strategy must be one of the following:
		- "best" (best-fit)
		- "worst" (worst-fit)
		- "first" (first-fit)
		- "next" (next-fit)
   sz specifies the number of bytes that will be available, in total, for all mymalloc requests.
*/

void initmem(strategies strategy, size_t sz)
{
    myStrategy = strategy;

    /* all implementations will need an actual block of memory to use */
    //printf("Initing mem with size %zu\n",sz);
    mySize = sz;

    /* TODO: release any other memory you were using for bookkeeping when doing a re-initialization! */
    if (head != NULL)
        free(head);
    if (lastVisited != NULL)
        free(lastVisited);
    if (myMemory != NULL)
        free(myMemory);

    /* TODO: Initialize memory management structure. */
    myMemory = malloc(sz);

    head = (struct memoryList *)malloc(sizeof(struct memoryList));
    head->last = NULL; // No link before head yet
    head->next = NULL; // No link after head yet
    head->size = sz; // assign it all the space available
    head->alloc = 0; // It is not yet allocated
    head->ptr = myMemory;

    //For the "next fit" we need to keep track of lastVisited
    lastVisited = head;
}

/* Allocate a block of memory with the requested size.
 *  If the requested block is not available, mymalloc returns NULL.
 *  Otherwise, it returns a pointer to the newly allocated block.
 *  Restriction: requested >= 1
 */

void *mymalloc(size_t requested)
{
    /* DEBUG */
    //printf("\n\nRequested a block of size %zu\n",requested);

    assert((int)myStrategy > 0);
    void *ptr;
    switch (myStrategy)
    {
        case NotSet:
            return NULL;
        case First:
            ptr = malloc_first(requested);
            //printf("MEM after alloc----------------------------------------------------------------\n");
            //print_memory();
            return ptr;
        case Best:
            return NULL;
        case Worst:
            return NULL;
        case Next:
            return NULL;
    }
    return NULL;
}


/* Frees a block of memory previously allocated by mymalloc. */
void myfree(void* block)
{
    struct memoryList *node = head;

    while (node)
    {
        //If node has the correct memory
        if (node->ptr == block){
            //Remove the node and free it's memory
            //printf("Removing a block of size: %ld\n",node->size);
            freeNode(node);
            //printf("MEM after removal----------------------------------------------------------------\n");
            //print_memory();
            return;
        }
        node = node->next;
    }
}

/****** Memory status/property functions ******
 * Implement these functions.
 * Note that when refered to "memory" here, it is meant that the
 * memory pool this module manages via initmem/mymalloc/myfree.
 */

/* Get the number of contiguous areas of free space in memory. */
int mem_holes()
{
    int holes = 0;
    struct memoryList *node = head;
    while (node){
        if (node->alloc == 0){
            holes++;
        }
        node = node->next;
    }
    return holes;
}

/* Get the number of bytes allocated */
int mem_allocated()
{
    int bytesAllocated = 0;
    struct memoryList *node = head;
    while (node){
        if (node->alloc == 1){
            bytesAllocated += node->size;
        }
        node = node->next;
    }
    return bytesAllocated;
}

/* Number of non-allocated bytes */
int mem_free()
{
    int bytesFree = 0;
    struct memoryList *node = head;
    while (node){
        if (node->alloc == 0){
            bytesFree += node->size;
        }
        node = node->next;
    }
    return bytesFree;
}

/* Number of bytes in the largest contiguous area of unallocated memory */
int mem_largest_free()
{
    int largestFree = 0;
    struct memoryList *node = head;
    while (node){
        if (node->alloc == 0 && node->size > largestFree){
            largestFree = node->size;
        }
        node = node->next;
    }
    return largestFree;
}

/* Number of free blocks smaller than "size" bytes. */
int mem_small_free(int size)
{
    int numOfSmallFree = 0;
    struct memoryList *node = head;
    while (node){
        if (node->alloc == 0 && node->size <= size){
            numOfSmallFree++;
        }
        node = node->next;
    }
    return numOfSmallFree;
}

char mem_is_alloc(void *ptr)
{
    struct memoryList *node = head;
    while (node){
        if (node->alloc == 1 && node->ptr == ptr){
            return 1;
        } else if (node->alloc == 0 && node->ptr == ptr){
            return 0;
        }
        node = node->next;
    }
    return 0;
}

/*
 * Feel free to use these functions, but do not modify them.
 * The test code uses them, but you may find them useful.
 */


//Returns a pointer to the memory pool.
void *mem_pool()
{
    return myMemory;
}

// Returns the total number of bytes in the memory pool. */
int mem_total()
{
    return mySize;
}


// Get string name for a strategy.
char *strategy_name(strategies strategy)
{
    switch (strategy)
    {
        case Best:
            return "best";
        case Worst:
            return "worst";
        case First:
            return "first";
        case Next:
            return "next";
        default:
            return "unknown";
    }
}

// Get strategy from name.
strategies strategyFromString(char * strategy)
{
    if (!strcmp(strategy,"best"))
    {
        return Best;
    }
    else if (!strcmp(strategy,"worst"))
    {
        return Worst;
    }
    else if (!strcmp(strategy,"first"))
    {
        return First;
    }
    else if (!strcmp(strategy,"next"))
    {
        return Next;
    }
    else
    {
        return 0;
    }
}


/*
 * These functions are for you to modify however you see fit.  These will not
 * be used in tests, but you may find them useful for debugging.
 */

void printNode(struct memoryList *node)
{
    printf("Node{\nNext: %p\nLast: %p\nSize: %zu\nAlloc: %d\nPtr: %p\n}\n",node->next,node->last,node->size,node->alloc,node->ptr);
}

/* Use this function to print out the current contents of memory. */
void print_memory()
{
    struct memoryList *currentNode = head;
    while (currentNode != NULL){
        //Print the node
        printNode(currentNode);
        //update
        currentNode = currentNode->next;
    }

    return;
}



/* Use this function to track memory allocation performance.
 * This function does not depend on your implementation,
 * but on the functions you wrote above.
 */
void print_memory_status()
{
    printf("%d out of %d bytes allocated.\n",mem_allocated(),mem_total());
    printf("%d bytes are free in %d holes; maximum allocatable block is %d bytes.\n",mem_free(),mem_holes(),mem_largest_free());
    printf("Average hole size is %f.\n\n",((float)mem_free())/mem_holes());
}

/* Use this function to see what happens when your malloc and free
 * implementations are called.  Run "mem -try <args>" to call this function.
 * We have given you a simple example to start.
 */
void try_mymem(int argc, char **argv) {
    strategies strat;
    void *a, *b, *c, *d, *e;
    if(argc > 1)
        strat = strategyFromString(argv[1]);
    else
        strat = First;


    /* A simple example.
       Each algorithm should produce a different layout. */
/*
    initmem(strat,500);

    a = mymalloc(100);
    b = mymalloc(100);
    c = mymalloc(100);
    myfree(b);
    d = mymalloc(50);
    myfree(a);
    e = mymalloc(25);
*/
    void* first;
    void* second;
    void* third;
    int correctThird;

    initmem(strat,100);

    first = mymalloc(10);
    second = mymalloc(1);
    myfree(first);
    third = mymalloc(1);
    printf("Done with test. Printing memory status\n");
    print_memory();
    print_memory_status();


}

//------------------------Utility functions---------------------------------
/**
Inserts newNode after oldNode
Doesnt change memory allocation.
 */
void insertNodeAfter(struct memoryList *oldNode, struct memoryList *newNode ){
    newNode->next = oldNode->next;
    newNode->last = oldNode;
    oldNode->next = newNode;
}

/*
 The memory that the node is holding should be freed.
 Node should be merged with any surrounding free nodes
 */
void freeNode(struct memoryList *node){
    // Mark that this node is no longer allocated and free it's memory
    node->alloc = 0;

    //Check if it should be merged with "left" neighbor
    int mergeLeft = node->last != NULL && node->last->alloc == 0;
    if (mergeLeft){
        //printf("Merging nodes %p and %p\n",node,node->last);
        mergeFreeNodes(node->last,node);
    }

    //Check if it should be merged with "right" neighbor
    int mergeRight = node->next != NULL && node->next->alloc == 0;
    if (mergeRight){
        //printf("Merging nodes %p and %p\n",node,node->next);
        mergeFreeNodes(node,node->next);
    }
}

/*
 Makes 2 free nodes into 1 free node of total size
 The 2 nodes should be neighbors and should both be free (not allocated)
 */
void mergeFreeNodes(struct memoryList *firstNode, struct memoryList *lastNode){
    //Assert that firstNode and lastNode are indeed neighbors
    if (firstNode->next != lastNode && lastNode->last != firstNode ){
        printf("Error in mergeFreeNodes(). Nodes are not neighbors");
        return;
    }
    if (firstNode->alloc != 0 && lastNode->alloc != 0){
        printf("Error in mergeFreeNodes(). Nodes are not not free");
        return;
    }
    //Calculate new size
    size_t newSize = firstNode->size + lastNode->size;

    //Remove the last node
    removeNode(lastNode);

    //Update first node (ptr doesn't need to be updated)
    firstNode->size = newSize; //Set new size
}

void removeNode(struct memoryList *node){
    struct memoryList *myLast = node->last;
    struct memoryList *myNext = node->next;

    //If node is head, make head point to next node
    if (node == head){
        printf("Removing head node\n");
        head = head->next;
    }

    //Make last point to next
    if (myLast){ //NULL pointer check
        myLast->next = myNext;
    }

    //Make next point to last
    if (myNext){ //NULL pointer check
        myNext->last = myLast;
    }

    //Free node
    free(node);
}

void *allocOnNode(struct memoryList *node, size_t requested){
    if (node->size == requested){ //If size fits excactly
        node->alloc = 1;
    } else { //requested < node->size
        //Create new node for remaining space
        size_t remainingSize = node->size - requested;
        void *remainingMemory = node->ptr + requested;
        struct memoryList *remainingNode = malloc(sizeof(struct memoryList));
        remainingNode->last = NULL;
        remainingNode->next = NULL;
        remainingNode->size = remainingSize;
        remainingNode->alloc = 0;
        remainingNode->ptr = remainingMemory;
        insertNodeAfter(node,remainingNode);

        //Update node
        node->alloc = 1;
        node->size = requested;
    }
    return node->ptr;
}

//-------------------Malloc functions--------------------------------------
void *malloc_first(size_t requested){
    lastVisited = head;

    while (lastVisited)
    {
        //If node is free and is big enough
        int isFree = lastVisited->alloc == 0;
        int isBigEnough = lastVisited->size >= requested;
        if (isFree && isBigEnough){
            return allocOnNode(lastVisited,requested);
        }
        lastVisited = lastVisited->next;
    }

    return NULL;
}


