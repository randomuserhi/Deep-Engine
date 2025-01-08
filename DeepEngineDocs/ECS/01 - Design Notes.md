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

## Technical details

- Archetype tree
	- Stores entities of a specific type combination and creates a tree (in real time) of how to traverse between archetypes when components are inserted or removed 

- Entity references