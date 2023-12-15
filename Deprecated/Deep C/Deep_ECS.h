#ifndef h_Deep_ECS
#define h_Deep_ECS

#include "Deep.h"
#include "Deep_Containers.h"

#define DEEP_ECS_NULL 0 //Null entity
#define DEEP_ECS_COMPONENT 256 //0b100000000 //EntityHandle given to Components
#define DEEP_ECS_ID 512 //0b1000000000 //Component used for providing a name to entities (for readability)

#define DEEP_ECS_ENTITY_MASK 0b1111111111111111111111111111111111111111111111111111111100000000 //Mask to provide Entity ID handle
#define DEEP_ECS_CHILDOF 0b1 //This entity is a child of <EntityHandle> provided
#define DEEP_ECS_INSTANCEOF 0b10 //This entity is an instance of <EntityHandle> provided

typedef uint64_t Deep_ECS_Handle;

#ifndef Deep_DynArray_Decl_Deep_ECS_Handle
#define Deep_DynArray_Decl_Deep_ECS_Handle
Deep_DynArray_Decl(Deep_ECS_Handle, Deep_ECS_Handle)
#endif

#ifndef Deep_DynArray_Decl_Deep_DynArray_raw
#define Deep_DynArray_Decl_Deep_DynArray_raw
Deep_DynArray_Decl(struct Deep_DynArray(raw), Deep_DynArray_raw)
#endif

struct Deep_ECS_Archetype_Edge
{
	struct Deep_ECS_Archetype* add;
	struct Deep_ECS_Archetype* remove;
};

#ifndef Deep_UnorderedMap_Decl_Deep_ECS_Handle_Deep_ECS_Archetype_Edge
#define Deep_UnorderedMap_Decl_Deep_ECS_Handle_Deep_ECS_Archetype_Edge
Deep_UnorderedMap_Decl(Deep_ECS_Handle, struct Deep_ECS_Archetype_Edge, Deep_ECS_Handle, Deep_ECS_Archetype_Edge)
#endif

struct Deep_ECS_Reference
{
	struct Deep_ECS_Archetype* archetype;
	size_t index;
};

#ifndef Deep_UnorderedMap_Decl_Deep_ECS_Handle_To_Deep_ECS_Reference
#define Deep_UnorderedMap_Decl_Deep_ECS_Handle_To_Deep_ECS_Reference
Deep_UnorderedMap_Decl(Deep_ECS_Handle, struct Deep_ECS_Reference, Deep_ECS_Handle, Deep_ECS_Reference)
#endif

typedef uint64_t Deep_ECS_ArchetypeHash;

struct Deep_ECS_Archetype
{
	struct Deep_DynArray(Deep_ECS_Handle) type;
	size_t size;
	struct Deep_DynArray(Deep_ECS_Handle) handles;
	struct Deep_DynArray(Deep_DynArray_raw) components;

	struct Deep_UnorderedMap(Deep_ECS_Handle, Deep_ECS_Archetype_Edge) edges;
};

#ifndef Deep_UnorderedMap_Decl_Deep_DynArray_Deep_ECS_Handle_To_Deep_ECS_Archetype
#define Deep_UnorderedMap_Decl_Deep_DynArray_Deep_ECS_Handle_To_Deep_ECS_Archetype
Deep_UnorderedMap_Decl(struct Deep_DynArray(Deep_ECS_Handle), struct Deep_ECS_Archetype, Deep_DynArray_Deep_ECS_Handle, Deep_ECS_Archetype)
#endif


#ifndef Deep_UnorderedMap_Decl_Deep_ECS_Handle_To_Deep_ECS_Archetype_Ptr
#define Deep_UnorderedMap_Decl_Deep_ECS_Handle_To_Deep_ECS_Archetype_Ptr
Deep_UnorderedMap_Decl(Deep_ECS_Handle, struct Deep_ECS_Archetype*, Deep_ECS_Handle, Deep_ECS_Archetype_Ptr)
#endif

struct Deep_ECS_Component
{
	size_t size; // sizeof(Component)
};

struct Deep_ECS_Id
{
	const char* name;
};

struct Deep_ECS
{
	struct Deep_UnorderedMap(Deep_ECS_Handle, Deep_ECS_Reference) hierarchy;
	struct Deep_UnorderedMap(Deep_DynArray_Deep_ECS_Handle, Deep_ECS_Archetype) archetypes;
	struct Deep_ECS_Archetype root;
};

Deep_ECS_ArchetypeHash Deep_ECS_Archetype_Hash(const Deep_ECS_Handle* archetypeType, size_t size);
void Deep_ECS_Archetype_Create(struct Deep_ECS_Archetype* archetype);
void Deep_ECS_Archetype_Free(struct Deep_ECS_Archetype* archetype);
struct Deep_ECS_Reference Deep_ECS_Archetype_Push(struct Deep_ECS* ECS, struct Deep_ECS_Archetype* archetype);

void Deep_ECS_Create(struct Deep_ECS* ECS);
void Deep_ECS_Free(struct Deep_ECS* ECS);
void Deep_ECS_CreateComponent(struct Deep_ECS* ECS, const char* name, size_t componentSize);
struct Deep_ECS_Archetype* Deep_ECS_GetArchetype(struct Deep_ECS* ECS, const Deep_ECS_Handle* type, size_t typeLength);
void Deep_ECS_PrintHierarchy(struct Deep_ECS* ECS);

#endif
