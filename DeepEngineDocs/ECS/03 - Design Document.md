> ECDB (Entity-Component Database) is a database as opposed to a full ECS implementation.
> 
> It does not automatically run systems but rather provides a mechanism for storing, referencing and querying entities.
> 
> It is up to the developer to create systems and execute them on these queries etc...
> - This gives more control to the developer over a full ECS system following the design philosophy of flexibility and "choose your own toppings" solutions.
> - The developer has to manage concurrent systems and threaded access themselves.

**Notes**

The implementation works at compile-time *and* runtime to support modding where a developer may expose an API that custom scripts, native (**dll**'s) or external (**wasm**, **lua**, etc...) can use to access ECDB. 
- This is why types use a `ComponentId` instead of `std::type_index` (or alternatives) such that the *id* of types remains constant across environments and such that non c++ types can be registered such as those from external custom scripts like **lua** / **wasm**
- If the runtime support is not required by a given implementation, the developer can write a wrapper that maps `std::type_index` to the runtime `ComponentId`.
	- Deep provides an example implementation of this as `class ECRegistry::StaticTypeMap`
	- (This mapping can realistically be anything, even a string name as opposed to `std::type_index`)
	- *Performance is not an issue as majority of lookups to types are only performed once (and not during the main program loop), for example queries will only go through the mapping on creation. Once constructed, the query does not use any custom lookup and works with `ComponentId` directly.*

**Operations** (All of which are thread-safe)
- [x] Register Components/Tag types
- [x] Create/Delete Entity
- [x] Add/Remove Components from Entities
- [x] Add/Remove Tags to Entities
- [x] Reference Specific Entities
- [ ] Queries
- [ ] Register Shared Components
- [ ] Add/Remove Shared Components

> NOTE:: 
> Deregistering types has many caveats with entities that are currently using a component/tag as well as `ECDB` allocated chunks / archetypes that get invalidated.
> 
> As a result, deregistering types as a concept is fundamentally flawed. A better approach is to deconstruct the entire `ECDB` and reconstruct it again with the right types.
- [ ] Deregister Components/Tag types
- [ ] Deregister Shared Components
### ECRegistry

Manages components / tag types used within ECDB. Registering a type returns an *integer* handle which is used to identify the component.

You may optionally provide a `name` to the component which is used in debugging.
- Typically the `typename` is used (*a macro is provided that gives this to you*)

> NOTE:: For the compile time variants, since the type has no correlation to the runtime, registering the same type will return a new component handle, treating the duplicate type as a different component.

```cpp
using ComponentId = int;

class ECRegistry {
	ComponentId RegisterComponent(size_t size, size_t alignment, char* name = nullptr);
	Deep_Inline template<typename T> ComponentId RegisterComponent(char* name = nullptr);
	
	Deep_Inline ComponentId RegisterTag(char* name = nullptr);
	Deep_Inline template<typename T> ComponentId RegisterTag(char* name = nullptr);

	int RequestComponent(ComponentId component, size_t size, size_t alignment, char* name = nullptr)
	Deep_Inline template<typename T> int RequestComponent(ComponentId component, char* name = nullptr)
	
	Deep_Inline int RequestTag(ComponentId component, char* name = nullptr)
	Deep_Inline template<typename T> int RequestTag(ComponentId component, char* name = nullptr)
}
```

`ComponentId` is an integer value representing the component. All references to the type will use the component id.

You may also specify what `ComponentId` value you wish to give a component/tag via:
- `RequestComponent`
- `RequestTag`

This becomes important when optimizing [[03 - Design Document#Archetypes|archetype lookup]].
#### Components

Blocks of data that can be assigned to entities.

```cpp
ComponentId ECRegistry::RegisterComponent(size_t size, size_t alignment, char* name = nullptr);
ComponentId ECRegistry::RegisterComponent<typename T>(char* name = nullptr); // compile-time variant (must be trivially constructable)
```

> NOTE:: Using `RegisterComponent<T>` where type `T` has no members or member functions, is equivalent to `RegisterTag<T>`
#### Tags

Tags are components with 0 data.

```cpp
ComponentId ECRegistry::RegisterTag(char* name = nullptr);
ComponentId ECRegistry::RegisterTag<typename T>(char* name = nullptr); // compile-time variant (size of type must be 0)
```

> NOTE:: These are simply wrappers around `RegisterComponent` where `size` is 0
### ECDB

```cpp
class ECDB {
	ECDB();
	ECDB(ECRegistry registry);
}
```
#### Archetypes

An archetype describes the *type* of an entity which is simply what components / tags the entity is composed of.

Internally this *type* is represented as a special bitfield which supports an arbitrary number of bits (Each $i$'th bit specifies whether component $i$ is in the archetype).
- Simply an internal `std::vector<uintXX>` which is indexed as a 2d array of bits.
- `XX` is either 32 or 64 depending on system platform.

Entities are stored in archetypes such that entities made up of the same components / tags are concurrent in memory.

**Chunks**
- Each archetype allocates entities in chunks, this is to prevent having to move entities during a resize operation and instead chunks can be allocated/reused.
- These chunks utilize a [Free List](https://en.wikipedia.org/wiki/Free_list) to manage reuse of old chunks
- These chunks are of fixed size (*not count*) such that they offer a good balance for cache efficiency (Probably follow Unity's footsteps with 16kib chunk sizes):
	- *Small Chunks* - Too many chunks can result in overhead crossing chunk borders (pointer chasing)
	- *Large Chunks* - Loading a single chunk can evict other parts of cache that may have benefitted from being around as well as being wasteful if not the entire chunk is utilized

**Operations**
- Archetype equality => To check if 2 entities are from the same archetype, simply check if they are part of the same archetype container (entities themselves do not store their *type* but rather are contained within the same container and the container stores the *type*)
- Has Component/Tag => To check if an entity / the archetype has a given component, check the *type* bitfield
#### Add/Remove Components/Tags from Entities

*This is effectively changing an entities type.*

Each archetype maintains a lookup for which archetype entities would move to when a given component is added. This produces a graph which can be traversed to reach the final archetype.
- The graph is constructed at run time
#### Create/Delete Entities

When an entity is created it does not belong to an archetype and holds no data. It does, however, have a valid handle which can reference it (despite it not existing in memory, since the entity has no data).

Once a component/tag is added the entity is allocated inside of the appropriate archetype and chunk. (It occupies a chunk with free space and is appended to the last free slot).

On deletion, or removal from an archetype given a type change, the entity is deallocated and the last entity in the archetype is moved to fill the gap created by the removed entity.
#### Entity References/Handles

There is a lookup table which stores the location of entities in memory. This lookup is updated whenever an entity is moved (either by archetype change or other).

The handle / reference then points to the item in the lookup table which is fixed per entity (until the entity is deleted in which the reference becomes invalid)

Use of Entity Versioning and Index recycling: https://www.youtube.com/watch?v=gaj4SY4KNR4
- versioning prevents accidental reuse of an entity which has changed due to a move either from deletion or change in archetype
#### Queries

### Modding and Portability

As mentioned before, `ECDB` is implemented to work at runtime to support modding. For this reason, components are not tied to types as they can have differences in struct layout across DLL boundaries (or environments). 

Instead, `ECDB` acts as an allocator for the memory the components reside in and pointers are passed around instead. This way the actual use of the component (struct layout and all) can remain isolated preventing portability issues. The DLL that defines the layout of the component can optionally expose an API for reading the members of said memory region externally.

> NOTE:: For instances where this is not applicable, a variant that returns a typed memory region is also provided: 
> - `void* GetComponent(ComponentId comp)`
> - `Deep_Inline T& GetComponent<typename T>(ComponentId comp)`

Alternatively, modding can be done by loading a DLL compiled with the exact same ABI and the same struct packing and alignment (typically safely achieved by compiling both the custom DLL and the main code with the same compiler (including compiler version + patch)) thus allowing the raw structs to be passed around.

Or just recompile the entire program with the custom code from scratch!


