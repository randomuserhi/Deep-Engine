#include "Deep_ECS.h"

//This hash functions doesn't quite support 64 bit values => Needs changing
Deep_ECS_ArchetypeHash Deep_ECS_Archetype_Hash(const struct Deep_DynArray(Deep_ECS_Handle)* type)
{
	Deep_ECS_ArchetypeHash Seed = type->size;
	for (size_t i = 0; i < type->size; i++)
	{
		Seed ^= type->values[0] + 0x9e3779b9 + (Seed << 6) + (Seed >> 2);
	}
	return Seed;
}

void Deep_ECS_Archetype_Create(struct Deep_ECS_Archetype* archetype)
{
	Deep_DynArray_Create(Deep_ECS_Handle)(&archetype->type);
	Deep_DynArray_Create(Deep_ECS_Handle)(&archetype->handles);
	Deep_DynArray_Create(Deep_DynArray_raw)(&archetype->components);
	Deep_UnorderedMap_Create(Deep_ECS_Handle, Deep_ECS_Archetype_Edge)(&archetype->edges, Deep_UnorderedMap_ByteCompare);

	archetype->size = 0;
}

void Deep_ECS_Archetype_Free(struct Deep_ECS_Archetype* archetype)
{
	Deep_DynArray_Free(Deep_ECS_Handle)(&archetype->type);
	Deep_DynArray_Free(Deep_ECS_Handle)(&archetype->handles);
	for (size_t i = 0; i < archetype->components.size; i++)
	{
		Deep_DynArray_Free(raw)(archetype->components.values + i);
	}
	Deep_DynArray_Free(Deep_DynArray_raw)(&archetype->components);
	Deep_UnorderedMap_Free(Deep_ECS_Handle, Deep_ECS_Archetype_Edge)(&archetype->edges);
}

struct Deep_ECS_Reference Deep_ECS_Archetype_Push(struct Deep_ECS* ECS, struct Deep_ECS_Archetype* archetype)
{
	//TODO:: The checking here to see if the type exists can be done during archetype creation
	//		 and stored in the archetype as an array alongside the type.
	//       This should help performance specially when pushing lots of new entities to the archetype.

	Deep_ECS_Handle* key = archetype->type.values;
	for (size_t i = 0; i < archetype->type.size; ++i, ++key)
	{
		size_t hash = Deep_UnorderedMap_Hash(key, sizeof * key, DEEP_UNORDEREDMAP_SEED);
		struct Deep_ECS_Reference* typeReference = Deep_UnorderedMap_Find(Deep_ECS_Handle, Deep_ECS_Reference)(&ECS->hierarchy, hash, key);
		
		if (typeReference != NULL)
		{
			Deep_DynArray_Push(raw)(archetype->components.values + i);
		}
	}

	struct Deep_ECS_Reference reference;
	reference.archetype = archetype;
	reference.index = archetype->size++;
	return reference;
}
