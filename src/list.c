#include "list.h"
#include "error.h"
#include <stdlib.h>
#include <stdbool.h>

struct List listNew(size_t elemSize, size_t alloc){
	struct List c = {.elementSize = elemSize, .allocStep = alloc};
	return c;
}

void listAdd(struct List* list, const void* data, size_t count){
	for(size_t dataidx = 0; dataidx < count; ++dataidx){
		// if no more space add elements
		if(list->data == NULL || list->elementSize * list->elementCount >= list->bytesAllocated){
			void* temp = realloc(list->data, list->bytesAllocated + list->allocStep * list->elementSize);
			testError(!temp, "failed to reallocate space for list");
			list->data = temp;
			list->bytesAllocated += list->allocStep * list->elementSize;
		}
		// copy into array per byte
		unsigned char* insertPoint = (unsigned char*)list->data + list->elementCount * list->elementSize;
		for(size_t byteidx = 0; byteidx < list->elementSize; ++byteidx){
			insertPoint[byteidx] = ((unsigned char*)data)[byteidx + dataidx * list->elementSize];
		}
		++list->elementCount;
		}
}

void* listAt(struct List list, size_t idx){
	if(!list.data || idx >= list.elementCount){
		return NULL;
	}
	return (unsigned char*)list.data + idx * list.elementSize;
}

void* listBeg(struct List list){
	return list.data;
}

void* listEnd(struct List list){
	// return NULL if no data or end
	return list.data == NULL ? NULL : (unsigned char*)list.data + list.elementSize * list.elementCount;
}

void listZero(struct List* list){
	if(list->data){
		free(list->data);
		list->data = NULL;
	}
	list->elementCount = 0;
	list->bytesAllocated = 0;
}
