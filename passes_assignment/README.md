### Task for passes

#### Part 1
1. count the insturctions in a functions
2. count the basic blocks
3. find the basic block with maximum instructions
4. find the basic block with maximum successors and predecessors

#### Part 2 
1.  Introduce a global variable G to the module and store 0 in the entry block of function.
2. Store integers 0 to G in entry block. In all other block store 1,2,3.. to G.
3. Store 0 to G in entry block. In every other block get G, exclusive or with integers 1,2,3... and store back in G.