 Memory Management
===================

The problem will focus on memory.  You will implement your own 
version of malloc() and free(), using a variety of allocation strategies.

You will be implementing a memory manager for a block of memory.  You will
implement routines for allocating and deallocating memory, and keeping track of
what memory is in use.  You will implement one of the four strategies for selecting in
which block to place a new requested memory block:

  1) First-fit: select the first suitable block with smallest address.
  2) Best-fit: select the smallest suitable block.
  3) Worst-fit: select the largest suitable block.
  4) Next-fit: select the first suitable block after
     the last block allocated (with wraparound
     from end to beginning).


Here, "suitable" means "free, and large enough to fit the new data".

Here are the functions you will need to implement:

initmem():
  Initialize memory structures.

mymalloc():
  Like malloc(), this allocates a new block of memory.

myfree():
  Like free(), this deallocates a block of memory.

mem_holes():
  How many free blocks are in memory?

mem_allocated():
  How much memory is currently allocated?

mem_free():
  How much memory is NOT allocated?

mem_largest_free():
  How large is the largest free block?

mem_small_free():
  How many small unallocated blocks are currently in memory?

mem_is_alloc():
  Is a particular byte allocated or not?

A structure has been provided for use to implement these functions.  It is a
doubly-linked list of blocks in memory (both allocated and free blocks).  Every
malloc and free can create new blocks, or combine existing blocks.  You may
modify this structure, or even use a different one entirely.  However, do not
change function prototypes or files other than mymem.c.

IMPORTANT NOTE: Regardless of how you implement memory management, make sure
that there are no adjacent free blocks.  Any such blocks should be merged into
one large block.

A few functions are given to help you monitor what happens when you
call your functions.  Most important is the try_mymem() function.  If you run
your code with "mem -try <args>", it will call this function, which you can use
to demonstrate the effects of your memory operations.  These functions have no
effect on test code, so use them to your advantage.

Running your code:

After running "make", run

1) "mem" to see the available tests and strategies.
2) "mem -test <test> <strategy>" to test your code with provided tests.
3) "mem -try <args>" to run your code with your own tests
   (the try_mymem function).

You can also use "make test" and "make stage1-test" for testing.  "make
stage1-test" only runs the tests relevant to stage 1.

Running "mem -test -f0 ..." will allow tests to run even
after previous tests have failed.  Similarly, using "all" for a test or strategy
name runs all of the tests or strategies.  Note that if "all" is selected as the
strategy, the 4 tests are shown as one.

One of the tests, "stress", runs an assortment of randomized tests on each
strategy.  The results of the tests are placed in "tests.out" .  You may want to
view this file to see the relative performance of each strategy.


Stage 1
-------

Implement all the above functions, for all the 4 strategy in a group
Use "mem -test all first" to test your implementation


Stage 2
-------
you should answer the 10 questions asked below together in a group.
The strategy is passed to initmem(), and stored in the global variable "myStrategy".
Some of your functions will need to check this variable to implement the
correct strategy.

You can test your code with "mem -test all worst", etc., or test all 4 together
with "mem -test all all".  The latter command does not test the strategies
separately; your code passes the test only if all four strategies pass.


Answer the following questions as part of your report
=====================================================


1) Why is it so important that adjacent free blocks not be left as such?
What would happen if they were permitted?
It is important, so we can correctly keep track of the available blocks of sequential memory.
Example: There are two adjacent blocks of size A and B.
We are looking for a block of size X=A+B.
If the adjacent blocks are not merged, we will not be able to find a fit for X,
since they are represented as two individual blocks.

2) Which function(s) need to be concerned about adjacent free blocks?
In our design, we only need to be concerned about free blocks in myfree(), when
we find the block that we want to free. Here, we find out, if any of it's
adjacent blocks are also free, in which case they should be merged.
This is actually the only case, where adjacent free blocks can occur, if we handle it this way.
Therefore, we don't need to be concerned about adjacent free blocks anywhere else.

3) Name one advantage of each strategy.
First:
It runs quickly since it doesn't need to go through the whole list every time.

Next:
It runs even quicker than first, since it doesn't need to go through the whole list every time,
and because if we allocate a lot of blocks after each other, we don't need to iterate through the
same allocated blocks each time.

Best:
It minimizes how often the big blocks are split, which minimizes the number of failed allocations

Worst:
The average number of small blocks is smaller than in the other strategies, because
we rarely get small "remaining blocks" after splitting a block in two.



4) Run the stress test on all strategies, and look at the results (tests.out).
What is the significance of "Average largest free block"?
It is nice to have some very large blocks available, so we have the possibility
to allocate large sizes of memory. This helps in avoiding failed allocations of large blocks.

Which strategy generally has the best performance in this metric?
In the most tests, it is the "best" strategy,

Why do you think this is?
It only splits the largest blocks when it is absolutely necessary.



5) In the stress test results (see Question 4), what is the significance of
"Average number of small blocks"?
It indicates the number of very small blocks, that are generally not very useful, but
can be seen a "wasted memory", since you will likely not be able to allocate anything in them.

Which strategy generally has the best performance in this metric?
The "Worst" strategy generally has the smallest number in this metric, which is the best
since we want to minimize the number of small blocks.

Why do you think this is?
This is because we rarely get small "remaining blocks" after splitting a block in two,
if we always choose to split the largest block.


6) Eventually, the many mallocs and frees produces many small blocks scattered
across the memory pool.  There may be enough space to allocate a new block, but
not in one place.  It is possible to compact the memory, so all the free blocks
are moved to one large free block.  How would you implement this in the system
you have built?

We could run through the memory list from head to tail sequentially.
When we encounter a free block, we switch it with it's "next" block (which is alloced).
If we make a switch so that two free blocks are next to each other, we merge them.
This way, we can run through the memory list once, and we don't need a buffer for the
moved blocks. This is because, we only switch used blocks with free blocks
(not used blocks with used blocks), so we never overwrite used data.

7) If you did implement memory compaction, what changes would you need to make
in how such a system is invoked (i.e. from a user's perspective)?
We could make it invisible to the user, by running the compaction algorithm periodically,
or by running it every time we got a failed allocation, and then trying to do the allocation again.



8) How would you use the system you have built to implement realloc?  (Brief
explanation; no code)
This could be done by simply calling myfree on the old memory and then mymalloc
with the new requested size.
If we wish for realloc to copy the content from the old memory into the new memory,
we could mark the old memory as free (alloc=0) but not acutally free the memory
until we found our new node, where we could then move it to.

9) Which function(s) need to know which strategy is being used?  Briefly explain
why this/these and not others.

mymalloc:
The "mymalloc" function obvious needs to know which strategy is being used so it can call the correct function.
Depending on the strategy, different functions are called with different algorithms which determine where the
the new block of memory should be placed.

The "myfree" function does not need to know the strategy. There is always just one correct
node for myfree to find, so the only thing we could optimize is how fast we found this node.
This was not a part of the assignment, so we have not done it.


10) Give one advantage of implementing memory management using a linked list
over a bit array, where every bit tells whether its corresponding byte is
allocated.

It's faster to find a free block which matches the size of the block that is needed.
You don't need to count if there is enough free bytes, and therefore it's faster to find where
the block of data should be placed.



