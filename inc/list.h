#ifndef LIST_H
#define LIST_H

#include <stddef.h>

// dynamic array-like structure that allows element insertion at the end and random access
struct List{
	size_t elementCount;	// number of elements in array
	size_t elementSize;	// size in bytes of each element
	size_t bytesAllocated;	// number of elements memory has been allocated for
	size_t allocStep;	// number of extra elements to allocate after space runs out
	void* data;		// pointer to the array
};

// create a new list with some elements and set elementSize
struct List listNew(size_t elemSize, size_t alloc);

// add count element from data to list
void listAdd(struct List* list, const void* data, size_t count);

// return pointer to the data array at index idx (with respect to element byte size)
void* listAt(struct List list, size_t idx);

// return pointer to the beginning of the array
void* listBeg(struct List list);

// return pointer to the end of the array
void* listEnd(struct List list);

// free memory allocated and reset length and allocated count
void listZero(struct List* list);

#endif
