#include "Deep.h"

#define GIGA_BYTE (1024 * 1024 * 1024)

#ifdef malloc
#undef malloc
#endif

#ifdef free
#undef free
#endif

size_t mallocCount = 0;
size_t freeCount = 0;

//TODO:: implement this as a linked list
size_t mallocListIndex = 0;
void* mallocList[200];

void* Deep_Malloc(size_t size, const char* file, int line, const char* function)
{
	++mallocCount;
	void* p = malloc(size);

	mallocList[mallocListIndex++] = p;

	printf("Allocated[ %s ] { %s, %i, %p[%zu] }\n", function, file, line, p, size);
	return p;
}

void Deep_Free(void* ptr, const char* file, int line, const char* function)
{
	++freeCount;
	
	if (!ptr)
	{
		printf("Freed[ %s ] { %s, %i, NULL PTR }\n", function, file, line);
		return;
	}

	for (size_t i = 0; i < mallocListIndex; i++)
	{
		if (mallocList[i] == ptr)
		{
			free(ptr);
			printf("Freed[ %s ] { %s, %i, %p }\n", function, file, line, ptr);
			mallocList[i] = NULL;
			return;
		}
	}

	printf("Could not free[ %s ] { %s, %i, %p }\n", function, file, line, ptr);
}

void Deep_CheckMaxAllocationSize()
{
	unsigned long long mallocSize = 0, numGigaBytes = 0;
	void* mallocMemory = NULL;

	do
	{
		mallocSize += GIGA_BYTE;
		numGigaBytes = mallocSize / GIGA_BYTE;
		mallocMemory = malloc(mallocSize);
		if (mallocMemory)
		{
			printf("Dynamically allocated %llu GBs\n", numGigaBytes);
			free(mallocMemory);
		}
		else
		{
			printf("Failed to allocate %llu GBs\n", numGigaBytes);
			break;
		}
	} while (1);
}
