#include "Deep_Debug_Memory.h"

// https://stackoverflow.com/a/9074274/9642458
#if defined(DEEP_DEBUG_MEMORY)

/*
* Implementation of dictionary for pointers for memory allocation debugging.
*/

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

#define DEEP_DEBUG_MEMORY_ALLOCATIONDICT_BUCKETSIZE 13
#define DEEP_DEBUG_MEMORY_ALLOCATIONDICT_LOADFACTOR 0.7
#define DEEP_DEBUG_MEMORY_ALLOCATIONDICT_GROWTHRATE 2

struct Deep_Debug_Memory_AllocationDictSlot
{
	struct Deep_Debug_Memory_AllocationDictSlot* _next;
	struct Deep_Debug_Memory_AllocationDictSlot* next;
	struct Deep_Debug_Memory_AllocationDictSlot* prev;

	const void* allocation;
	const char* file;
	int line;
	const char* function;
};

struct Deep_Debug_Memory_AllocationDict
{
	struct Deep_Debug_Memory_AllocationDictSlot* start;
	struct Deep_Debug_Memory_AllocationDictSlot* end;
	struct Deep_Debug_Memory_AllocationDictSlot** hashes;
	size_t bucketSize;
	size_t size;
};

struct Deep_Debug_Memory_AllocationDict allocations = { NULL, NULL, NULL, 0, 0 };

void Deep_Debug_Memory_AllocationDict_Init()
{
	allocations.start = NULL;
	allocations.end = NULL;
	allocations.hashes = calloc(DEEP_DEBUG_MEMORY_ALLOCATIONDICT_BUCKETSIZE, sizeof * allocations.hashes);
	allocations.bucketSize = DEEP_DEBUG_MEMORY_ALLOCATIONDICT_BUCKETSIZE;
	allocations.size = 0;
}

void Deep_Debug_Memory_AllocationDict_Free()
{
	if (allocations.size > 0)
	{
		printf("(DEEP_DEBUG_MEMORY) Warning, not all allocations were freed: \n");
		Deep_Debug_Memory_PrintAllocations();
	}

	for (struct Deep_Debug_Memory_AllocationDictSlot* hashSlot = allocations.start; hashSlot != NULL;)
	{
		struct Deep_Debug_Memory_AllocationDictSlot* Old = hashSlot;
		hashSlot = hashSlot->next;
		free(Old);
	}
	free(allocations.hashes);

	allocations.start = NULL;
	allocations.end = NULL;
	allocations.hashes = NULL;
	allocations.bucketSize = 0;
	allocations.size = 0;
}

void Deep_Debug_Memory_AllocationDict_ReHash()
{
	if (allocations.hashes)
	{
		if (allocations.size / allocations.bucketSize > DEEP_DEBUG_MEMORY_ALLOCATIONDICT_LOADFACTOR)
		{
			const size_t newBucketSize = allocations.bucketSize * DEEP_DEBUG_MEMORY_ALLOCATIONDICT_GROWTHRATE;
			void* tmp = calloc(newBucketSize, sizeof * allocations.hashes);
			if (tmp)
			{
				free(allocations.hashes);
				allocations.bucketSize = newBucketSize;
				allocations.hashes = tmp;
				for (struct Deep_Debug_Memory_AllocationDictSlot* hashSlot = allocations.start; hashSlot != NULL; hashSlot = hashSlot->next)
				{
					const size_t index = (uintptr_t)hashSlot->allocation % allocations.bucketSize;
					if (allocations.hashes[index] == NULL)
					{
						allocations.hashes[index] = hashSlot;
					}
					else
					{
						struct Deep_Debug_Memory_AllocationDictSlot* tmp = allocations.hashes[index];
						while (tmp->_next)
						{
							tmp = tmp->_next;
						}
						tmp->_next = hashSlot;
					}
					hashSlot->_next = NULL;
				}
			}
		}
	}
}

#ifdef Deep_Compiler_GCC
/*
* Parameter key should be "const void*", but that causes GCC to write a completely valid [-Wmaybe-uninitialized] warning.
* This is because Deep_Debug_Memory_Malloc will perform "void* p = malloc(size)", creating a block of uninitialized memory.
* When this function is called, as the parameter key is "const void*" GCC knows that the assigned value at "*key" won't change,
* and thus during this functions execution, the value remains uninitialized, thus throwing the error.
*
* This is fine, however the implementation doesn't dereference or use the value at "*key" ever, so the warning should not
* apply.
*/
void Deep_Debug_Memory_AllocationDict_Insert(void* key, const char* file, int line, const char* function)
#else
void Deep_Debug_Memory_AllocationDict_Insert(const void* key, const char* file, int line, const char* function)
#endif
{
	if (allocations.hashes)
	{
		Deep_Debug_Memory_AllocationDict_ReHash();

		const size_t index = (uintptr_t)key % allocations.bucketSize;
		if (allocations.hashes[index] == NULL)
		{
			void* tmp = malloc(sizeof * allocations.start);
			if (tmp)
			{
				allocations.hashes[index] = tmp;
				allocations.hashes[index]->allocation = key;
				allocations.hashes[index]->file = file;
				allocations.hashes[index]->line = line;
				allocations.hashes[index]->function = function;
				allocations.hashes[index]->_next = NULL;
				allocations.hashes[index]->next = NULL;
				allocations.hashes[index]->prev = allocations.end;
				allocations.size++;
				if (allocations.end != NULL)
				{
					allocations.end->next = allocations.hashes[index];
				}
				allocations.end = allocations.hashes[index];
				if (allocations.start == NULL)
					allocations.start = allocations.hashes[index];
			}
		}
		else
		{
			struct Deep_Debug_Memory_AllocationDictSlot* hashSlot = allocations.hashes[index];
			struct Deep_Debug_Memory_AllocationDictSlot* prevSlot = NULL;
			while (hashSlot)
			{
				if (hashSlot->allocation == key)
				{
					//Maybe alert of duplicate ?
					return;
				}

				prevSlot = hashSlot;
				hashSlot = hashSlot->_next;
			}
			void* tmp = malloc(sizeof * allocations.start);
			if (tmp)
			{
				prevSlot->_next = tmp;
				prevSlot->_next->allocation = key;
				prevSlot->_next->file = file;
				prevSlot->_next->line = line;
				prevSlot->_next->function = function;
				prevSlot->_next->next = NULL;
				prevSlot->_next->_next = NULL;
				prevSlot->_next->prev = allocations.end;
				allocations.size++;
				if (allocations.end != NULL)
					allocations.end->next = prevSlot->_next;
				allocations.end = prevSlot->_next;
			}
		}
	}
}

int Deep_Debug_Memory_AllocationDict_Find(const void* key)
{
	if (allocations.hashes)
	{
		const size_t index = (uintptr_t)key % allocations.bucketSize;
		if (allocations.hashes[index] != NULL)
		{
			struct Deep_Debug_Memory_AllocationDictSlot* hashSlot = allocations.hashes[index];
			while (hashSlot)
			{
				if (hashSlot->allocation == key)
				{
					return DEEP_TRUE;
				}

				hashSlot = hashSlot->_next;
			}
		}
	}
	return DEEP_FALSE;
}

void Deep_Debug_Memory_AllocationDict_Erase(void* allocation)
{
	if (allocations.hashes)
	{
		const size_t index = (uintptr_t)allocation % allocations.bucketSize;
		if (allocations.hashes[index] != NULL)
		{
			struct Deep_Debug_Memory_AllocationDictSlot* hashSlot = allocations.hashes[index];
			struct Deep_Debug_Memory_AllocationDictSlot* prevhashSlot = NULL;
			while (hashSlot)
			{
				if (hashSlot->allocation == allocation)
				{
					if (prevhashSlot)
						prevhashSlot->_next = hashSlot->_next;
					else
						allocations.hashes[index] = hashSlot->_next;

					if (hashSlot->prev)
						hashSlot->prev->next = hashSlot->next;
					else // If prev is null, then we are altering first element and the start of linked list needs to be reset
					{
						allocations.start = hashSlot->next;
						if (hashSlot->next)
							hashSlot->next->prev = NULL;
					}
					if (hashSlot->next)
						hashSlot->next->prev = hashSlot->prev;
					else // If next is null, then we are altering last element and the end of linked list needs to be reset
					{
						allocations.end = hashSlot->prev;
						if (hashSlot->prev)
							hashSlot->prev->next = NULL;
					}

					free(hashSlot);
					allocations.size--;
					break;
				}

				prevhashSlot = hashSlot;
				hashSlot = hashSlot->_next;
			}
		}
	}
}

size_t mallocCount = 0;
size_t freeCount = 0;
size_t freeCountNoNull = 0;

void* Deep_Debug_Memory_Malloc(size_t size, const char* file, int line, const char* function)
{
	void* p = malloc(size);

	if (allocations.hashes)
	{
		if (p)
		{
			++mallocCount;

			Deep_Debug_Memory_AllocationDict_Insert(p, file, line, function);

#ifdef DEEP_DEBUG_MEMORY_VERBOSE
			printf("malloc => %s > %s : line(%i) ( %p[%zu] )\n", file, function, line, p, size);
#endif
		}
		else
		{
			printf("(DEEP_DEBUG_MEMORY) Warning, malloc returned NULL ptr => %s > %s : line(%i)", file, function, line);
		}
	}
	return p;
}

void* Deep_Debug_Memory_Calloc(size_t count, size_t typeSize, const char* file, int line, const char* function)
{
	void* p = calloc(count, typeSize);

	if (allocations.hashes)
	{
		if (p)
		{
			++mallocCount;

			Deep_Debug_Memory_AllocationDict_Insert(p, file, line, function);

#ifdef DEEP_DEBUG_MEMORY_VERBOSE
			printf("calloc => %s > %s : line(%i) ( %p[%zu] )\n", file, function, line, p, count);
#endif
		}
		else
		{
			printf("(DEEP_DEBUG_MEMORY) Warning, calloc returned NULL ptr => %s > %s : line(%i)", file, function, line);
		}
	}
	return p;
}

void* Deep_Debug_Memory_Realloc(void* ptr, size_t size, const char* file, int line, const char* function)
{
	void* p = realloc(ptr, size);

	if (allocations.hashes)
	{
		if (p)
		{
			if (!Deep_Debug_Memory_AllocationDict_Find(ptr))
			{
				++mallocCount;
#ifdef DEEP_DEBUG_MEMORY_VERBOSE
				printf("realloc => %s > %s : line(%i) ( .old = %p, .new = %p[%zu] )\n", file, function, line, ptr, p, size);
#endif
			}
			else
			{
				Deep_Debug_Memory_AllocationDict_Erase(ptr);
#ifdef DEEP_DEBUG_MEMORY_VERBOSE
				printf("realloc => %s > %s : line(%i) ( .old = %p, .new = %p[%zu] )\n", file, function, line, ptr, p, size);
#endif
			}
		}
		else
		{
			printf("(DEEP_DEBUG_MEMORY) Warning, realloc returned NULL ptr => %s > %s : line(%i)", file, function, line);
			return NULL;
		}
	}
	Deep_Debug_Memory_AllocationDict_Insert(p, file, line, function);
	return p;
}

void Deep_Debug_Memory_Free(void* ptr, const char* file, int line, const char* function)
{
	free(ptr);
	if (allocations.hashes)
	{
		++freeCount;

		if (!ptr)
		{
#ifdef DEEP_DEBUG_MEMORY_VERBOSE
			printf("free => %s > %s : line(%i) ( %p )\n", file, function, line, ptr);
#endif
			return;
		}
		else if (Deep_Debug_Memory_AllocationDict_Find(ptr))
		{
			++freeCountNoNull; 
#ifdef DEEP_DEBUG_MEMORY_VERBOSE
			printf("free => %s > %s : line(%i) ( %p )\n", file, function, line, ptr);
#endif
			Deep_Debug_Memory_AllocationDict_Erase(ptr);
		}
		else
			printf("(DEEP_DEBUG_MEMORY) Warning, invalid free => %s > %s : line(%i) ( %p )\n", file, function, line, ptr);
	}
}

void Deep_Debug_Memory_PrintAllocations()
{
	if (allocations.hashes)
	{
		printf("malloc: %zu\nfree (no null calls): %zu\nfree (all calls): %zu\n", mallocCount, freeCountNoNull, freeCount);

		printf("\nRemaining pointers: [%zu] { ", allocations.size);
		const char* seperator = "\n\t";
		for (struct Deep_Debug_Memory_AllocationDictSlot* slot = allocations.start; slot != NULL; slot = slot->next)
		{
			printf("%s[ %s > %s : line(%i) ( %p ) ]\n", seperator, slot->file, slot->function, slot->line, slot->allocation);
			seperator = "\t";
		}
		printf("}");
	}
	else
	{
		printf("(DEEP_DEBUG_MEMORY) Debugging never started, use Deep_Debug_Memory_Start().\n");
	}
}

void Deep_Debug_Memory_Start()
{
	int verbose = 0;
#ifdef DEEP_DEBUG_MEMORY_VERBOSE
	verbose = 1;
#endif
	printf("(DEEP_DEBUG_MEMORY) Started { .verbose = %i }\n", verbose);
	Deep_Debug_Memory_AllocationDict_Init();
}

void Deep_Debug_Memory_End()
{
	if (allocations.hashes)
	{
		printf("(DEEP_DEBUG_MEMORY) Ended.\n");
		Deep_Debug_Memory_AllocationDict_Free();
	}
	else
	{
		printf("(DEEP_DEBUG_MEMORY) Debugging never started, use Deep_Debug_Memory_Start().\n");
	}
}

#endif
