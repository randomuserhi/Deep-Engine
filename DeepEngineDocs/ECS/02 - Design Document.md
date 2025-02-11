### ECS Class

> ECS Class operates more like a Database than an actual full ECS framework.
> 
> It does not automatically run systems etc, only provides a mechanism for storing, referencing and querying entities.
> 
> It is up to the developer to create systems and execute them on these queries etc...
> This gives more control to the developer over a full ECS system following the design philosophy of flexibility and "choose your own toppings" solutions.

*Consider renaming to ECDB (Entity-Component Database)*

**Operations**
- Create/Delete Entity
- Add/Remove Components from Entities
- Add/Remove Tags to Entities
- Add/Remove Shared Components
- Query Entities
- Hierarchy of Entities
	- Set parent
	- Set child
	- Iterate children
	- Find parent

**Caveats**
- Must be runtime operated to support modding
### Components

Blocks of data that can be assigned to entities.
### Tags

Tags are components with 0 data.
### Archetypes

An archetype describes the *type* of an entity which is simply what components / tags the entity is composed of.

Internally this *type* is represented as a special bitfield which supports an arbitrary number of bits. This way groups of 32/64 components can easily be checked with number comparisons. (Each $i$'th bit specifies whether component $i$ is in the archetype).
- For efficiency, the bitfield supports sparsity in which an offset can be applied (undecided size, maybe a single byte is suffice?) to each 32/64 bit block.
- This means that for optimal usage, components used together for specific entities should be grouped to share a single 32/64 bit chunk so that at most 2 comparisons are needed to compare archetypes (one to check offset, and the second to compare the chunks) 

Entities are stored in archetypes such that entities made up of the same components / tags are concurrent in memory.

Each archetype maintains a lookup for which archetype entities would move to when a given component is added. This produces a graph which can be traversed to reach the final archetype.
- The graph is constructed at run time

