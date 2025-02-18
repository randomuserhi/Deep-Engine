### ECS Class

> ECS Class operates more like a Database than an actual full ECS framework.
> 
> It does not automatically run systems etc, only provides a mechanism for storing, referencing and querying entities.
> 
> It is up to the developer to create systems and execute them on these queries etc...
> This gives more control to the developer over a full ECS system following the design philosophy of flexibility and "choose your own toppings" solutions.

*Consider renaming to ECDB (Entity-Component Database)*

**Operations**
- [ ] Create/Delete Entity
- [ ] Add/Remove Components from Entities
- [ ] Add/Remove Tags to Entities
- [ ] Add/Remove Shared Components
- [ ] Query Entities
- [ ] Reference Specific Entities

**Caveats**
- Must be runtime operated to support modding
- Thread safety of operations
	- Maintain concurrency whenever possible to prevent waiting on locks constantly
### Components

Blocks of data that can be assigned to entities.
### Tags

Tags are components with 0 data.
### Shared Components

Blocks of data that are assigned to archetypes and shared across all entities.
### Archetypes

An archetype describes the *type* of an entity which is simply what components / tags the entity is composed of.

Internally this *type* is represented as a special bitfield which supports an arbitrary number of bits. This way groups of 32/64 components can easily be checked with number comparisons. (Each $i$'th bit specifies whether component $i$ is in the archetype).
- For efficiency, the bitfield supports sparsity in which an offset can be applied (undecided size, maybe a single byte is suffice?) to each 32/64 bit block.
	- Can even place the offset inside the 32/64 bit field, so each chunk stores a 8 bit offset and 24/56 bit field for the components
- This means that for optimal usage, components used together for specific entities should be grouped to share a single 32/64 bit chunk so that at most 2 comparisons are needed to compare archetypes (one to check offset, and the second to compare the chunks)
	- If the offset is placed inside the field, then only 1 comparison is needed
	- An API for the developer to specify component `id`'s to control this in someway is needed
	- Not very good for components shared across many archetypes
		- Transform component is needed in pretty much all entity types
		- Maybe use 32 bit chunks (24 bit field + 8 bit offset)
			- On 64 bit systems, common shared components can fit in the first 32 bit chunk and the second 32 bit chunk fits the entity components
			- This way a single 64 bit comparison can be done to check archetype despite spanning 2 chunks
		- Another strategy is 16 bit chunks (12 bit field + 4 bit offset)
			- 32 bit systems can fit 2 chunks then, 64 bit 4 chunks.
			- This is effectively the same as 32 bit chunks as 2 16 bit chunks make up the same footprint.

Entities are stored in archetypes such that entities made up of the same components / tags are concurrent in memory.

Each archetype maintains a lookup for which archetype entities would move to when a given component is added. This produces a graph which can be traversed to reach the final archetype.
- The graph is constructed at run time

**Adding Entities**
*How are entities stored within archetypes - as a single array? chunks?*
- https://docs.unity3d.com/Packages/com.unity.entities@1.0/manual/concepts-archetypes.html#archetype-chunks

**Removing Entities**
- Entities are removed and the last entity in the archetype is moved to fill the gap produced

### Entity References / Handles

There is a lookup table which stores the location of entities in memory. This lookup is updated whenever an entity is moved (either by archetype change or other).

The handle / reference then points to the item in the lookup table which is fixed per entity (until the entity is deleted in which the reference becomes invalid)

Use of Entity Versioning and Index recycling: https://www.youtube.com/watch?v=gaj4SY4KNR4
- versioning prevents accidental reuse of an entity which has changed due to a move (refer to *Removing Entities*)
### A Note on Hierarchies

ECS will not support hierarchies inherently. This is because hierarchies are better suited for scene graphs and rendering, even physics engines do not need hierarchies, and it does not make sense for them to have hierarchies.

If you are combining this ECS (which is more of a database) to a scene graph you would implement components that reference the scene graph and have systems that synchronize entities with their scene graph counterparts (ECS components act as a wrapper around the scene graph)
- However you map the ECS data to scene graph is implementation dependent and up to the developer.
	- An example implementation may require a copy of data in ECS and then copying that information or just a handle to the object in the scene graph (like a pointer). This object can then be queried using the scene graph for the information required.

**Additional Reading (kind of related)**
https://gamedev.stackexchange.com/questions/206715/hierarchical-relationships-in-an-entity-component-system
https://gamedev.stackexchange.com/questions/174319/dealing-with-more-complex-entities-in-an-ecs-architecture
https://gamedev.stackexchange.com/questions/203541/branchless-archetype-ecs/203562#203562
https://www.youtube.com/watch?v=rX0ItVEVjHc
https://jrouwe.nl/architectingjolt/ArchitectingJoltPhysics_Rouwe_Jorrit_Notes.pdf
