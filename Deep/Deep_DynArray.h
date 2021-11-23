#ifndef h_Deep_DynArray
#define h_Deep_DynArray

#include "Deep.h"

#define DEEP_DYNARRAY_GROWTHRATE 1.5f //Growthrate for dynamic arrays

#define DEEP_DYNARRAY_FREE_ON_ERROR 1
#define DEEP_DYNARRAY_KEEP_ON_ERROR 0

struct Deep_DynArray_Options
{
	char freeOnError; // If set, will free the contents of the array when an error is caught, otherwise the contents remain
};

struct Deep_DynArray_raw
{
	size_t size; // Number of elements in array (to optimize out division)
	size_t capacity; // Capacity of array (not number of elements in array)
	unsigned char* values; // Pointer to data
	struct Deep_DynArray_Options options; // Array options

	size_t typeSize; // sizeof(type)
};

Deep_Inline void Deep_DynArray_raw_Create(struct Deep_DynArray_raw* arr, size_t typeSize)
{
	arr->values = NULL;
	arr->size = 0;
	arr->capacity = 0;
	arr->typeSize = typeSize;
	arr->options.freeOnError = DEEP_DYNARRAY_FREE_ON_ERROR;
}
void Deep_DynArray_raw_Free(struct Deep_DynArray_raw* arr);
Deep_Inline void Deep_DynArray_raw_ErrorFree(struct Deep_DynArray_raw* arr)
{
	if (arr->options.freeOnError)
	{
		free(arr->values);
		arr->values = NULL;
	}
}
Deep_Inline void* Deep_DynArray_raw_Push(struct Deep_DynArray_raw* arr)
{
	if (arr->values)
	{
		if (arr->size == arr->capacity)
		{
			size_t newCapacity = (size_t)(arr->capacity * DEEP_DYNARRAY_GROWTHRATE);
			if (newCapacity == arr->capacity) ++newCapacity;
			void* tmp = realloc(arr->values, arr->typeSize * newCapacity);
			if (tmp)
			{
				arr->values = tmp;
				arr->capacity = newCapacity;
				++arr->size;
				return arr->values + arr->typeSize * (arr->size - 1);
			}
			else
			{
				Deep_DynArray_raw_ErrorFree(arr);
				return NULL;
			}
		}
		else
		{
			++arr->size;
			return arr->values + arr->typeSize * (arr->size - 1);
		}
	}
	else
	{
		void* tmp = malloc(arr->typeSize);
		if (tmp)
		{
			arr->values = tmp;
			arr->size = 1;
			arr->capacity = 1;
			return arr->values;
		}
		else return NULL;
	}
}
void Deep_DynArray_raw_Pop(struct Deep_DynArray_raw* arr);
void Deep_DynArray_raw_RemoveAt(struct Deep_DynArray_raw* arr, size_t index);
void Deep_DynArray_raw_Shrink(struct Deep_DynArray_raw* arr);
void Deep_DynArray_raw_Reserve(struct Deep_DynArray_raw* arr, size_t size);

/*
* Defines a Deep_DynArray of type(tag).
*/
#define Deep_DynArray(tag) Deep_DynArray_##tag

/*
* Utility macros for getting functions for type(tag).
*/

#define Deep_DynArray_ReinterpretCast(tag) Deep_DynArray_##tag##_ReinterpretCast
#define Deep_DynArray_Create(tag) Deep_DynArray_##tag##_Create
#define Deep_DynArray_Free(tag) Deep_DynArray_##tag##_Free
#define Deep_DynArray_Push(tag) Deep_DynArray_##tag##_Push
#define Deep_DynArray_Pop(tag) Deep_DynArray_##tag##_Pop
#define Deep_DynArray_RemoveAt(tag) Deep_DynArray_##tag##_RemoveAt
#define Deep_DynArray_Shrink(tag) Deep_DynArray_##tag##_Shrink 
#define Deep_DynArray_Reserve(tag) Deep_DynArray_##tag##_Reserve
#define Deep_DynArray_Get(tag) Deep_DynArray_##tag##_Get

#define Deep_DynArray_Decl(type, tag) \
struct Deep_DynArray(tag) \
{ \
	union \
	{ \
		struct Deep_DynArray_raw raw; \
		struct \
		{  \
			size_t size; \
			size_t capacity; \
			type* values; \
			struct Deep_DynArray_Options options; \
		}; \
	}; \
}; \
Deep_Inline void Deep_DynArray_##tag##_Create(struct Deep_DynArray(tag)* arr) \
{ \
	Deep_DynArray_raw_Create(&arr->raw, sizeof(type)); \
} \
Deep_Inline struct Deep_DynArray(tag) Deep_DynArray_##tag##_ReinterpretCast(void* arr) \
{ \
	struct Deep_DynArray(tag) dst; \
	memcpy(&dst, arr, sizeof dst); \
	return dst; \
} \
Deep_Inline void Deep_DynArray_##tag##_Free(struct Deep_DynArray(tag)* arr) \
{ \
	Deep_DynArray_raw_Free(&arr->raw); \
} \
Deep_Inline void Deep_DynArray_##tag##_Pop(struct Deep_DynArray(tag)* arr) \
{ \
	Deep_DynArray_raw_Pop(&arr->raw); \
} \
Deep_Inline void Deep_DynArray_##tag##_RemoveAt(struct Deep_DynArray(tag)* arr, size_t index) \
{ \
	Deep_DynArray_raw_RemoveAt(&arr->raw, index); \
} \
Deep_Inline void Deep_DynArray_##tag##_Shrink(struct Deep_DynArray(tag)* arr) \
{ \
	Deep_DynArray_raw_Shrink(&arr->raw); \
} \
Deep_Inline void Deep_DynArray_##tag##_Reserve(struct Deep_DynArray(tag)* arr, size_t size) \
{ \
	Deep_DynArray_raw_Reserve(&arr->raw, size); \
} \
Deep_Inline type* Deep_DynArray_##tag##_Push(struct Deep_DynArray(tag)* arr) \
{ \
	if (arr->raw.typeSize == sizeof(type)) return Deep_DynArray_raw_Push(&arr->raw); \
	else Deep_Unreachable; \
} \
Deep_Inline type* Deep_DynArray_##tag##_Get(const struct Deep_DynArray(tag)* arr, size_t index) \
{ \
	return arr->values + index; \
}

Deep_Inline struct Deep_DynArray_raw Deep_DynArray_raw_ReinterpretCast(void* arr)
{
	struct Deep_DynArray(raw) dst;
	memcpy(&dst, arr, sizeof dst);
	return dst;
}

Deep_Inline void* Deep_DynArray_raw_Get(const struct Deep_DynArray_raw* arr, size_t index)
{
	return arr->values + arr->typeSize * index;
}

#endif
