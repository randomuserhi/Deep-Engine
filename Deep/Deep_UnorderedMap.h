#ifndef h_Deep_UnorderedMap
#define h_Deep_UnorderedMap

#include "Deep.h"

//TODO:: Refactor to not use the strainge _next attribute and work with a flag similar to std::unorderedMap

//TODO:: make this be random everytime program boots / something (Should not be random per hash function)
#define DEEP_UNORDEREDMAP_SEED 0xee448401
#define DEEP_UNORDEREDMAP_BUCKETSIZE 13
#define DEEP_UNORDEREDMAP_LOADFACTOR 0.7
#define DEEP_UNORDEREDMAP_GROWTHRATE 2
#define DEEP_UNORDEREDMAP_EMPTY 0

struct Deep_UnorderedMap_HashSlot
{
	struct Deep_UnorderedMap_HashSlot* _next;
	struct Deep_UnorderedMap_HashSlot* next;
	struct Deep_UnorderedMap_HashSlot* prev;
	size_t hash;
};

struct Deep_UnorderedMap_raw_To_raw
{
	struct Deep_UnorderedMap_HashSlot** hashes;
	struct Deep_UnorderedMap_HashSlot* start;
	struct Deep_UnorderedMap_HashSlot* end;
	size_t bucketSize;
	size_t size;
	int (*keyCompareFunc)(const void*, const void*, size_t);
	size_t keyTypeSize;
	size_t valueTypeSize;
	size_t keyTypeAlignment;
	size_t valueTypeAlignment;
	size_t keyOffset;
	size_t valueOffset;
};

size_t Deep_UnorderedMap_Hash(const void* ptr, size_t len, size_t seed);
int Deep_UnorderedMap_ByteCompare(const void* hashKey, const void* hey, size_t keyTypeSize);
int Deep_UnorderedMap_DynArrayCompare(const void* hashKey, const void* hey, size_t keyTypeSize);

Deep_Inline void Deep_UnorderedMap_raw_To_raw_Create(struct Deep_UnorderedMap_raw_To_raw* unorderedMap, int (*keyCompareFunc)(const void*, const void*, size_t), size_t keyTypeSize, size_t keyTypeAlignment, size_t valueTypeSize, size_t valueTypeAlignment)
{
	unorderedMap->keyCompareFunc = keyCompareFunc;
	unorderedMap->keyTypeSize = keyTypeSize;
	unorderedMap->valueTypeSize = valueTypeSize;
	unorderedMap->valueTypeAlignment = valueTypeAlignment;
	unorderedMap->keyTypeAlignment = keyTypeAlignment;
	unorderedMap->bucketSize = DEEP_UNORDEREDMAP_BUCKETSIZE;
	unorderedMap->size = 0;

	// Calculate offset for Deep_UnorderedMap_HashSlot to be stored as a header to the key and value
	// refer to https://youtu.be/IAdLwUXRUvg?t=1716
	unorderedMap->keyOffset = (sizeof(**unorderedMap->hashes) + unorderedMap->keyTypeAlignment - 1) / unorderedMap->keyTypeAlignment * unorderedMap->keyTypeAlignment;
	unorderedMap->valueOffset = ((unorderedMap->keyOffset + sizeof(unorderedMap->keyTypeSize)) + unorderedMap->valueTypeAlignment - 1) / unorderedMap->valueTypeAlignment * unorderedMap->valueTypeAlignment;

	unorderedMap->hashes = calloc(DEEP_UNORDEREDMAP_BUCKETSIZE, sizeof(*unorderedMap->hashes));
	unorderedMap->start = NULL;
	unorderedMap->end = NULL;
}
void Deep_UnorderedMap_raw_To_raw_Free(struct Deep_UnorderedMap_raw_To_raw* unorderedMap);
void* Deep_UnorderedMap_raw_To_raw_Insert(struct Deep_UnorderedMap_raw_To_raw* unorderedMap, size_t hash, const void* key);
void* Deep_UnorderedMap_raw_To_raw_Find(struct Deep_UnorderedMap_raw_To_raw* unorderedMap, size_t hash, const void* key);
void Deep_UnorderedMap_raw_To_raw_Erase(struct Deep_UnorderedMap_raw_To_raw* unorderedMap, size_t hash, const void* key);

#define Deep_UnorderedMap(keyTag, valueTag) Deep_UnorderedMap_##keyTag##_To_##valueTag

/*
* Utility macros for getting functions for type(tag).
*/

#define Deep_UnorderedMap_Create(keyTag, valueTag) Deep_UnorderedMap_##keyTag##_To_##valueTag##_Create
#define Deep_UnorderedMap_Insert(keyTag, valueTag) Deep_UnorderedMap_##keyTag##_To_##valueTag##_Insert
#define Deep_UnorderedMap_Find(keyTag, valueTag) Deep_UnorderedMap_##keyTag##_To_##valueTag##_Find
#define Deep_UnorderedMap_Free(keyTag, valueTag) Deep_UnorderedMap_##keyTag##_To_##valueTag##_Free
#define Deep_UnorderedMap_Erase(keyTag, valueTag) Deep_UnorderedMap_##keyTag##_To_##valueTag##_Erase
#define Deep_UnorderedMap_Value(keyTag, valueTag) Deep_UnorderedMap_##keyTag##_To_##valueTag##_Value
#define Deep_UnorderedMap_Key(keyTag, valueTag) Deep_UnorderedMap_##keyTag##_To_##valueTag##_Key

#define Deep_UnorderedMap_Decl(keyType, valueType, keyTag, valueTag) \
struct Deep_UnorderedMap(keyTag, valueTag) \
{ \
	union \
	{ \
		struct Deep_UnorderedMap_raw_To_raw raw; \
		struct \
		{  \
			struct Deep_UnorderedMap_HashSlot** hashes; \
			struct Deep_UnorderedMap_HashSlot* start; \
			struct Deep_UnorderedMap_HashSlot* end; \
			size_t bucketSize; \
			size_t size; \
		}; \
	}; \
}; \
Deep_Inline void Deep_UnorderedMap_##keyTag##_To_##valueTag##_Free(struct Deep_UnorderedMap(keyTag, valueTag)* unorderedMap) \
{ \
	Deep_UnorderedMap_raw_To_raw_Free(&unorderedMap->raw); \
} \
Deep_Inline valueType* Deep_UnorderedMap_##keyTag##_To_##valueTag##_Insert(struct Deep_UnorderedMap(keyTag, valueTag)* unorderedMap, size_t hash, const keyType* key) \
{ \
	return Deep_UnorderedMap_raw_To_raw_Insert(&unorderedMap->raw, hash, key); \
} \
Deep_Inline valueType* Deep_UnorderedMap_##keyTag##_To_##valueTag##_Find(struct Deep_UnorderedMap(keyTag, valueTag)* unorderedMap, size_t hash, const keyType* key) \
{ \
	return Deep_UnorderedMap_raw_To_raw_Find(&unorderedMap->raw, hash, key); \
} \
Deep_Inline void Deep_UnorderedMap_##keyTag##_To_##valueTag##_Erase(struct Deep_UnorderedMap(keyTag, valueTag)* unorderedMap, size_t hash, const keyType* key) \
{ \
	Deep_UnorderedMap_raw_To_raw_Erase(&unorderedMap->raw, hash, key); \
} \
Deep_Inline valueType* Deep_UnorderedMap_##keyTag##_To_##valueTag##_Value(const struct Deep_UnorderedMap(keyTag, valueTag)* unorderedMap, const struct Deep_UnorderedMap_HashSlot* hashSlot) \
{ \
	return (valueType*)((unsigned char*)hashSlot + unorderedMap->raw.valueOffset); \
} \
Deep_Inline keyType* Deep_UnorderedMap_##keyTag##_To_##valueTag##_Key(const struct Deep_UnorderedMap(keyTag, valueTag)* unorderedMap, const struct Deep_UnorderedMap_HashSlot* hashSlot) \
{ \
	return (keyType*)((unsigned char*)hashSlot + unorderedMap->raw.keyOffset); \
} \
Deep_Inline void Deep_UnorderedMap_##keyTag##_To_##valueTag##_Create(struct Deep_UnorderedMap(keyTag, valueTag)* unorderedMap, int (*keyCompareFunc)(const void*, const void*, size_t)) \
{ \
	Deep_UnorderedMap_raw_To_raw_Create(&unorderedMap->raw, keyCompareFunc, sizeof(keyType), Deep_AlignOf(keyType), sizeof(valueType), Deep_AlignOf(valueType)); \
}

/*
* The following is used to define the "raw" version of Deep_DynArray
* which uses a custom Create function to assign sizeof(type).
*/

Deep_Inline void* Deep_UnorderedMap_raw_To_raw_Value(const struct Deep_UnorderedMap(raw, raw)* unorderedMap, const struct Deep_UnorderedMap_HashSlot* hashSlot) \
{
	return (unsigned char*)hashSlot + unorderedMap->valueOffset;
}
Deep_Inline void* Deep_UnorderedMap_raw_To_raw_Key(const struct Deep_UnorderedMap(raw, raw)* unorderedMap, const struct Deep_UnorderedMap_HashSlot* hashSlot) \
{
	return (unsigned char*)hashSlot + unorderedMap->keyOffset;
}

#endif
