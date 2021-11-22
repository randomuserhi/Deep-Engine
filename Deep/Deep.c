#include "Deep.h"

#define GIGA_BYTE (1024 * 1024 * 1024)

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
