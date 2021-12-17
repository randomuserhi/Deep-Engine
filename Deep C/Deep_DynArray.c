#include "Deep_DynArray.h"

extern Deep_Inline void Deep_DynArray_raw_Create(struct Deep_DynArray_raw* arr, size_t typeSize);

void Deep_DynArray_raw_Free(struct Deep_DynArray_raw* arr)
{
	if (arr) 
	{
		free(arr->values);
		arr->values = NULL;
	}
}

extern Deep_Inline void Deep_DynArray_raw_ErrorFree(struct Deep_DynArray_raw* arr);

extern Deep_Inline void* Deep_DynArray_raw_Push(struct Deep_DynArray_raw* arr);

void Deep_DynArray_raw_Pop(struct Deep_DynArray_raw* arr)
{
	if (arr->values)
	{
		if (arr->size > 0)
		{
			--arr->size;
		}
	}
}

void Deep_DynArray_raw_RemoveAt(struct Deep_DynArray_raw* arr, size_t index)
{
	if (arr->values)
	{
		if (arr->size > 1 && index > 0 && index < arr->size)
		{
			const size_t size = arr->size - 1 - index;
			if (size != 0) memmove(arr->values + index * arr->typeSize, arr->values + (index + 1) * arr->typeSize, size * arr->typeSize);
			--arr->size;
		}
	}
}

void Deep_DynArray_raw_Shrink(struct Deep_DynArray_raw* arr)
{
	if (arr->values)
	{
		const size_t newCapacity = arr->size;
		if (newCapacity != arr->capacity)
		{
			if (newCapacity != 0)
			{
				void* tmp = realloc(arr->values, arr->typeSize * newCapacity);
				if (tmp)
				{
					arr->values = tmp;
					arr->capacity = newCapacity;
				}
				else
				{
					Deep_DynArray_raw_ErrorFree(arr);
				}
			}
			else
			{
				free(arr->values);
				arr->values = NULL;
				arr->capacity = newCapacity;
			}
		}
	}
}

void Deep_DynArray_raw_Reserve(struct Deep_DynArray_raw* arr, size_t size)
{
	const size_t newCapacity = arr->size + size;
	if (newCapacity > arr->capacity)
	{
		void* tmp = realloc(arr->values, arr->typeSize * newCapacity);
		if (tmp)
		{
			arr->values = tmp;
			arr->capacity = newCapacity;
			arr->size = size;
		}
		else
		{
			Deep_DynArray_raw_ErrorFree(arr);
		}
	}
	else
	{
		arr->size = size;
	}
}
