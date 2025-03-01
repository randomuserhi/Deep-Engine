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
	- Deep engine provides an example implementation of this as `class ECRegistry::StaticTypeMap`
	- (This mapping can realistically be anything, even a string name as opposed to `std::type_index`)
	- *Performance is not an issue as majority of lookups to types are only performed once, for example queries will only go through the mapping on creation. Once constructed, the query does not use any custom lookup and works with `ComponentId` directly.*


**Operations**
- [ ] Create/Delete Entity
- [ ] Add/Remove Components from Entities
- [ ] Add/Remove Tags to Entities
- [ ] Add/Remove Shared Components
- [ ] Query Entities
- [ ] Reference Specific Entities

```cpp
class ECDB {
	ECDB();
	ECDB(ECRegistry registry);
}
```
### ECRegistry

Manages components / tag types used within ECDB. Registering a type returns an *integer* handle which is used to identify the component.

You may optionally provide a `name` to the component which is used in debugging.
- Typically the `typename` is used (*a macro is provided that gives this to you*)

> NOTE:: For the compile time variants, since the type has no correlation to the runtime, registering the same type will return a new component handle, treating the type as a different component.

```cpp
using ComponentId = int;

class ECRegistry {
	ComponentId RegisterComponent(size_t size, size_t alignment, char* name = nullptr);
	Deep_Inline template<typename T> ComponentId RegisterComponent(char* name = nullptr);
	
	Deep_Inline ComponentId RegisterTag(char* name = nullptr);
	Deep_Inline template<typename T> ComponentId RegisterTag(char* name = nullptr);
}
```

`ComponentId` is an integer value representing the component. All references to the type will use the component id.
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
ComponentId ECRegistry::RegisterTag<typename T>(char* name = nullptr); // compile-time variant (must have no members)
```

> NOTE:: These are simply wrappers around `RegisterComponent` where `size` is 0
### Archetypes

### Add/Remove Components/Tags
### Create/Delete Entities

### Entity References

### Queries



