#include "Deep.h"

#define GIGA_BYTE (1024 * 1024 * 1024)

#ifdef malloc
#undef malloc
#endif

#ifdef calloc
#undef calloc
#endif

#ifdef realloc
#undef realloc
#endif

#ifdef free
#undef free
#endif

size_t mallocCount = 0;
size_t freeCount = 0;
size_t freeCountNoNull = 0;

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

void* Deep_Calloc(size_t count, size_t typeSize, const char* file, int line, const char* function)
{
	++mallocCount;
	void* p = calloc(count, typeSize);

	mallocList[mallocListIndex++] = p;

	printf("Calloc Allocated[ %s ] { %s, %i, %p[%zu] }\n", function, file, line, p, count);
	return p;
}

void* Deep_Realloc(void* ptr, size_t size, const char* file, int line, const char* function)
{
	void* p = realloc(ptr, size);

	for (size_t i = 0; i < mallocListIndex; i++)
	{
		if (mallocList[i] == ptr)
		{
			printf("Realloc [ %s ] { %s, %i, %p[%zu], old: %p }\n", function, file, line, p, size, ptr);
			mallocList[i] = p;

			return p;
		}
	}

	++mallocCount;
	mallocList[mallocListIndex++] = p;
	printf("Realloc act as malloc [ %s ] { %s, %i, NULL PTR, old: %p }\n", function, file, line, ptr);

	return p;
}

void Deep_Free(void* ptr, const char* file, int line, const char* function)
{
	++freeCount;
	
	if (!ptr)
	{
		printf("Freed[ %s ] { %s, %i, NULL PTR }\n", function, file, line);
		free(ptr);
		return;
	}

	for (size_t i = 0; i < mallocListIndex; i++)
	{
		if (mallocList[i] == ptr)
		{
			++freeCountNoNull;
			free(ptr);
			printf("Freed[ %s ] { %s, %i, %p }\n", function, file, line, ptr);
			mallocList[i] = NULL;
			return;
		}
	}

	printf("Could not free[ %s ] { %s, %i, %p }\n", function, file, line, ptr);
}

void Deep_CheckAllocations()
{
	printf("Malloc count: %zu\nFree count (no null calls): %zu\nFree count (all calls): %zu\n", mallocCount, freeCountNoNull, freeCount);

	printf("Remaining pointers: [");
	char seperator = ' ';
	for (size_t i = 0; i < mallocListIndex; i++)
	{
		if (mallocList[i])
		{
			printf("%c %p", seperator, mallocList[i]);
			seperator = ',';
		}	
	}
	printf("  ]\n");
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
