# A_Better_Malloc
An malloc like memory allocator library that allocates memory like regular malloc but allows for partial memory deallocation.
While deallocating the memory, the user can choose to:
* Free the whole memory block.
* Partially free the memory block by specifying an address within the block. The memory area after the specified address will be deallocated.
