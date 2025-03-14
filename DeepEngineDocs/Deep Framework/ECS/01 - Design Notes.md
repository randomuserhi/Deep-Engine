## Minimal Requirements

- [ ] Concurrent read access to entities
- [ ] Single write access to entities 
	- *Probably to groups / chunks as opposed to single entities as locks are expensive*
- [ ] Systems that act upon groups of entities concurrently
- [ ] Entities are part of a database which can very quickly be queried
	- [ ] Query by archetype (what components the entity has)
- [ ] Hold references to entities
	- [ ] Components can store references to other entities
	- [ ] Access by reference should not break concurrent read/write rules nor deadlock application
- [ ] Cache friendly implementation
	- [ ] Archetype tree ?
## Random Notes

- Archetype tree
	- Stores entities of a specific type combination and creates a tree (in real time) of how to traverse between archetypes when components are inserted or removed 
	- Chunking -> archetype stores chunks of data (Refer to Unity ECS)
		- Chunks provide more efficient memory allocation

- Entity references
	- https://stackoverflow.com/questions/19385853/understanding-cache-friendly-data-oriented-objects-and-handles
		- Probably just maintain a list and when deleting entities in the middle, instead of shifting all entities on the right of the list back 1, move the last entity into the hole generated by the deleted entity.
			- https://docs.unity3d.com/Packages/com.unity.entities@1.0/manual/concepts-structural-changes.html
		- Entity Versioning and Index recycling: https://www.youtube.com/watch?v=gaj4SY4KNR4
			- versioning prevents accidental reuse of an entity which has changed
		- Filling the hole requires 2 indirection -> handle goes to lookup and lookup points to entity (this allows lookup to be updated when entity is moved instead of all handles being updated)
			- Thread safety here is a bummer

- Queries
	- Querying can be post-processed (aka built prior) and then updated on the fly similar to how adding removing archetypes works

## Example

> DoT damage on enemies
> 
> **Systems**
> - DoT damage
> 	- Does 5 damage every second idk
> - Enemy Dead
> 	- If enemy hp is $<= 0$ then remove enemy

**Operations**
- Ability to add DoT component to enemy (stores damage etc...)
- Ability to remove entity

> Enemy pathing towards target player
> 
> **Systems**
> - Target Pathing
> 	- Follow player

**Operations**
- Ability to bunch types of enemies that are targeting the same player? (not really sure how this scales...) Cause if targeting constantly changes it might be expensive...
	- What about queries based on properties: "All enemies with Target component where target == player1"
	- I think DOTS way of handling things as a database with queries and being able to store lookups is best...