#include "Deep_UnorderedMap.h"
#include "Deep_DynArray.h"

#if Deep_SizeOf_SizeT == 8
Deep_Inline size_t _Deep_UnorderedMap_LoadBytes(const char* p, int n)
{
	size_t result = 0;
	--n;
	do
		result = (result << 8) + (unsigned char)(p[n]);
	while (--n >= 0);
	return result;
}

size_t _Deep_UnorderedMap_ShiftMix(size_t v)
{
	return v ^ (v >> 47);
}
#endif

Deep_Inline size_t _Deep_UnorderedMap_UnalignedLoad(const char* p)
{
	/*
	* Can be done using bit shifts:
	* 
		uint64_t Ret = 0;
		Ret |= (uint64_t) V[0];
		Ret |= ((uint64_t) V[1]) << 8;
		Ret |= ((uint64_t) V[2]) << 16;
		Ret |= ((uint64_t) V[3]) << 24;
		Ret |= ((uint64_t) V[4]) << 32;
		Ret |= ((uint64_t) V[5]) << 40;
		Ret |= ((uint64_t) V[6]) << 48;
		Ret |= ((uint64_t) V[7]) << 56;
   */
	size_t result;
	memcpy(&result, p, sizeof(result));
	return result;
}

size_t Deep_UnorderedMap_Hash(const void* ptr, size_t len, size_t seed)
{
	// http://gcc.gnu.org/git/?p=gcc.git;a=blob_plain;f=libstdc%2b%2b-v3/libsupc%2b%2b/hash_bytes.cc;hb=HEAD
	// Murmur hash function for 32 bit size_t
#if Deep_SizeOf_SizeT == 4
	const size_t m = 0x5bd1e995;
	size_t hash = seed ^ len;
	const char* buf = ptr;

	// Mix 4 bytes at a time into the hash.
	while (len >= 4)
	{
		size_t k = unaligned_load(buf);
		k *= m;
		k ^= k >> 24;
		k *= m;
		hash *= m;
		hash ^= k;
		buf += 4;
		len -= 4;
	}

	// Handle the last few bytes of the input array.
	switch (len)
	{
	case 3:
		hash ^= (unsigned char)buf[2] << 16;
		[[gnu::fallthrough]];
	case 2:
		hash ^= (unsigned char)(buf[1]) << 8;
		[[gnu::fallthrough]];
	case 1:
		hash ^= (unsigned char)(buf[0]);
		hash *= m;
	};

	// Do a few final mixes of the hash.
	hash ^= hash >> 13;
	hash *= m;
	hash ^= hash >> 15;
	return hash;
#endif

#if Deep_SizeOf_SizeT == 8
	static const size_t mul = (((size_t)0xc6a4a793UL) << 32UL) + (size_t)0x5bd1e995UL;
	const char* const buf = ptr;

	// Remove the bytes not divisible by the sizeof(size_t).  This
	// allows the main loop to process the data as 64-bit integers.
	const size_t len_aligned = len & ~(size_t)0x7;
	const char* const end = buf + len_aligned;
	size_t hash = seed ^ (len * mul);
	for (const char* p = buf; p != end; p += 8)
	{
		const size_t data = _Deep_UnorderedMap_ShiftMix(_Deep_UnorderedMap_UnalignedLoad(p) * mul) * mul;
		hash ^= data;
		hash *= mul;
	}
	if ((len & 0x7) != 0)
	{
		const size_t data = _Deep_UnorderedMap_LoadBytes(end, len & 0x7);
		hash ^= data;
		hash *= mul;
	}
	hash = _Deep_UnorderedMap_ShiftMix(hash) * mul;
	hash = _Deep_UnorderedMap_ShiftMix(hash);
	return hash;
#endif
}

int Deep_UnorderedMap_ByteCompare(const void* hashKey, const void* key, size_t keyTypeSize)
{
	return (memcmp(hashKey, key, keyTypeSize) == 0);
}

int Deep_UnorderedMap_DynArrayCompare(const void* hashKey, const void* key, size_t keyTypeSize)
{
	if (hashKey && key)
	{
		const struct Deep_DynArray(raw)* hashKeyArr = hashKey;
		const struct Deep_DynArray(raw)* keyArr = key;

		if (hashKeyArr->size == keyArr->size && hashKeyArr->typeSize == keyArr->typeSize)
		{
			return (memcmp(hashKeyArr->values, keyArr->values, hashKeyArr->size * hashKeyArr->typeSize) == 0);
		}
	}
	return DEEP_FALSE;
}

extern Deep_Inline void Deep_UnorderedMap_raw_To_raw_Create(struct Deep_UnorderedMap_raw_To_raw* unorderedMap, int (*keyCompareFunc)(const void*, const void*, size_t), size_t keyTypeSize, size_t keyTypeAlignment, size_t valueTypeSize, size_t valueTypeAlignment);

void Deep_UnorderedMap_raw_To_raw_Free(struct Deep_UnorderedMap_raw_To_raw* unorderedMap)
{
	if (unorderedMap)
	{
		for (struct Deep_UnorderedMap_HashSlot* hashSlot = unorderedMap->start; hashSlot != NULL;)
		{
			struct Deep_UnorderedMap_HashSlot* Old = hashSlot;
			hashSlot = hashSlot->next;
			free(Old);
		}
		free(unorderedMap->hashes);
	}
}

void Deep_UnorderedMap_raw_To_raw_ReHash(struct Deep_UnorderedMap_raw_To_raw* unorderedMap)
{
	if (unorderedMap->size / unorderedMap->bucketSize > DEEP_UNORDEREDMAP_LOADFACTOR)
	{
		const size_t newBucketSize = unorderedMap->bucketSize * DEEP_UNORDEREDMAP_GROWTHRATE;
		void* tmp = calloc(newBucketSize, sizeof(*unorderedMap->hashes));
		if (tmp)
		{
			free(unorderedMap->hashes);
			unorderedMap->bucketSize = newBucketSize;
			unorderedMap->hashes = tmp;
			for (struct Deep_UnorderedMap_HashSlot* hashSlot = unorderedMap->start; hashSlot != NULL; hashSlot = hashSlot->next)
			{
				const size_t index = hashSlot->hash % unorderedMap->bucketSize;
				if (unorderedMap->hashes[index] == NULL)
				{
					unorderedMap->hashes[index] = hashSlot;
				}
				else
				{
					struct Deep_UnorderedMap_HashSlot* tmp = unorderedMap->hashes[index];
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

void* Deep_UnorderedMap_raw_To_raw_Insert(struct Deep_UnorderedMap_raw_To_raw* unorderedMap, size_t hash, const void* key)
{
	Deep_UnorderedMap_raw_To_raw_ReHash(unorderedMap);

	const size_t index = hash % unorderedMap->bucketSize;
	if (unorderedMap->hashes[index] == NULL)
	{
		void* tmp = malloc(unorderedMap->valueOffset + unorderedMap->valueTypeSize);
		if (tmp)
		{
			unorderedMap->hashes[index] = tmp;
			unorderedMap->hashes[index]->hash = hash;
			unorderedMap->hashes[index]->_next = NULL;
			unorderedMap->hashes[index]->next = NULL;
			unorderedMap->hashes[index]->prev = unorderedMap->end;
			unorderedMap->size++;
			if (unorderedMap->end != NULL)
			{
				unorderedMap->end->next = unorderedMap->hashes[index];
			}
			unorderedMap->end = unorderedMap->hashes[index];
			if (unorderedMap->start == NULL)
				unorderedMap->start = unorderedMap->hashes[index];
			
			//Set key
			memcpy((unsigned char*)unorderedMap->hashes[index] + unorderedMap->keyOffset, key, unorderedMap->keyTypeSize);

			return (unsigned char*)unorderedMap->hashes[index] + unorderedMap->valueOffset;
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		struct Deep_UnorderedMap_HashSlot* hashSlot = unorderedMap->hashes[index];
		struct Deep_UnorderedMap_HashSlot* prevSlot = NULL;
		while (hashSlot)
		{
			if (unorderedMap->keyCompareFunc((unsigned char*)hashSlot + unorderedMap->keyOffset, key, unorderedMap->keyTypeSize))
			{
				return (unsigned char*)hashSlot + unorderedMap->valueOffset;
			}

			prevSlot = hashSlot;
			hashSlot = hashSlot->_next;
		}
		unsigned char* tmp = malloc(unorderedMap->valueOffset + unorderedMap->valueTypeSize);
		if (tmp)
		{
			prevSlot->_next = (void*)tmp;
			prevSlot->_next->hash = hash;
			prevSlot->_next->next = NULL;
			prevSlot->_next->_next = NULL;
			prevSlot->_next->prev = unorderedMap->end;
			unorderedMap->size++;
			if (unorderedMap->end != NULL)
				unorderedMap->end->next = prevSlot->_next;
			unorderedMap->end = prevSlot->_next;

			//Set key
			memcpy((unsigned char*)prevSlot->_next + unorderedMap->keyOffset, key, unorderedMap->keyTypeSize);

			return (unsigned char*)prevSlot->_next + unorderedMap->valueOffset;
		}
		return NULL;
	}
}

void* Deep_UnorderedMap_raw_To_raw_Find(struct Deep_UnorderedMap_raw_To_raw* unorderedMap, size_t hash, const void* key)
{
	const size_t index = hash % unorderedMap->bucketSize;
	if (unorderedMap->hashes[index] != NULL)
	{
		struct Deep_UnorderedMap_HashSlot* hashSlot = unorderedMap->hashes[index];
		while (hashSlot)
		{
			if (unorderedMap->keyCompareFunc((unsigned char*)hashSlot + unorderedMap->keyOffset, key, unorderedMap->keyTypeSize))
			{
				return (unsigned char*)hashSlot + unorderedMap->valueOffset;
			}

			hashSlot = hashSlot->_next;
		}
	}
	return NULL;
}

void Deep_UnorderedMap_raw_To_raw_Erase(struct Deep_UnorderedMap_raw_To_raw* unorderedMap, size_t hash, const void* key)
{
	const size_t index = hash % unorderedMap->bucketSize;
	if (unorderedMap->hashes[index] != NULL)
	{
		struct Deep_UnorderedMap_HashSlot* hashSlot = unorderedMap->hashes[index];
		struct Deep_UnorderedMap_HashSlot* prevhashSlot = NULL;
		while (hashSlot)
		{
			if (unorderedMap->keyCompareFunc((unsigned char*)hashSlot + unorderedMap->keyOffset, key, unorderedMap->keyTypeSize))
			{
				if (prevhashSlot)
					prevhashSlot->_next = hashSlot->_next;
				else
					unorderedMap->hashes[index] = hashSlot->_next;

				if (hashSlot->prev)
					hashSlot->prev->next = hashSlot->next;
				else // If prev is null, then we are altering first element and the start of linked list needs to be reset
				{
					unorderedMap->start = hashSlot->next;
					if (hashSlot->next)
						hashSlot->next->prev = NULL;
				}
				if (hashSlot->next)
					hashSlot->next->prev = hashSlot->prev;
				else // If next is null, then we are altering last element and the end of linked list needs to be reset
				{
					unorderedMap->end = hashSlot->prev;
					if (hashSlot->prev)
						hashSlot->prev->next = NULL;
				}

				free(hashSlot);
				unorderedMap->size--;
				break;
			}

			prevhashSlot = hashSlot;
			hashSlot = hashSlot->_next;
		}
	}
}
