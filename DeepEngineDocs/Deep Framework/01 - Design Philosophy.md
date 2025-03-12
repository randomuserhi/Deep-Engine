*Maybe rename to DeepFramework*

Deep Engine aims to be a framework for making game engines instead of a game engine itself:
- Follows the same design philosophy and [Psyshock](https://github.com/Dreaming381/Latios-Framework-Documentation/blob/main/Psyshock%20Physics/README.md) in which it offers low level access to the bare minimum and you create the pipeline yourself.
- This is the "choose your own toppings" solution in which the developer makes their own "pizza" over choosing a premade one.
	- `ECS` is not a full ECS system but instead is only the Entity Component Database supporting queries, referencing and storing but not systems at all. The developer instead uses the database operations and can create their own custom systems solution with their own pipeline. 
		- If not, the framework offers a systems solution -> but the point is that it remains modular and optional for those that want the specialised setup.
	- `Physics` is not a full physics engine but instead offers just math collision operations and solvers (as functions / premade systems) that you can hook up yourself to make a full physics simulation.
		- This removes the need to "hack" physics simulations to create your own handmade solutions for specific scenarios. Specifically character controllers.
			- Unity rigid body based character controllers are the bane of anyone's existence.

Heavily follows *Data-Oriented Design*. Refer to [Mike Acton's fantastic talk](https://youtu.be/rX0ItVEVjHc?t=660). 

/// Rewrite the below - maybe move somewhere else ///

- add a note about using multiple `ecdb` to manage data, eg ecdb for player / enemy logic and ecdb that manages physics
  - some synchronisation step is needed to sync position / velocity data from player ecdb to physics ecdb, but keeping them separate has the advantage of decreasing the slow move operations of changing entity archetype
  - eg if physics had a tag for asleep rigid bodies, by keeping this separate only the physics entity has to move archetype whilst the player archetype does not have to move but if they were combined an archetype needs to exist for any configuration of asleep or awake players for all player types like "bleeding players" etc...
  - aka less moves and less memory as less archetypes need to be allocated
  - thus is only an example and measurements should be done to see if the overhead of the synchronisation step is problematic
  - maybe add an api for a more sophisticated synchronisation step allowing for the order of entities in ecdb to match the order in the physics ecdb to prevent random access - this means swapping entities within archetypes to maintain order
    - this mitigates the performance gain from less swaps as physics ecdb is now reordering on each move from player ecdb, but the memory savings from less archetypes being allocated is the same 

- sync can be a simple copy operation using an entity reference in physics ecdb in player ecdb or vice versa
- or maintain order in physics ecdb and then access via indexing:

```
player ecdb: | arch0 | arch 1 |
physic ecdb: |       |        | 
(1 arch, but the order is as if arch0 and arch1 of player ecdb was concatenated)
```

add this in notes but not the final documentation
- when I finish implementing the basic ecs and get on to making the game and physics, then consider this approach as I'm implementing and testing performance

the key here is that we are not tied to ECS, we don't have to use a single ECS system to solve our problem and we do not have to make a single monolithic ECS database to house the entire logic of our problem

instead use ECS to solve what it's good for and use a second ECS database when they conflict

similarly hierarchies are better for rendering so why use a ECS system for a hierarchy? that's stupid.

ECS for logic (game or physics joints) then copy positional data of game entities to a hierarchy of rendered objects (bones) for rendering

why use ECS for ui? like u get the point.

///  Also rewrite this below part - maybe move somewhere else ///

KNOW UR DATA AND CONTEXT 

naive:
- deep currently has math operations all using SIMD horizontally within a structure
- More commonly SIMD should happen vertically, all x values of entity 1,2,3,4,5,6,7,8 (avx2 256 bit register float precision) get added, then y values etc... vs SIMD of a single xyz vector with another xyz vector such as when updating position and velocity

this is just naive and not true, in reality you are doing physics which often requires the set of x,y,z together horizontally collision detection etc all rely on xyz together - this is an example of not knowing ur data (edited)

the true vertical add would be: (avx2 256 bit register float precision) of 2 entities x1,y1,z1,w1,x2,y2,z2,w2 w1 and w2 are just 0 for vec3 position (in deep all vec3 are vec4)

/// Include these talks somewhere ///
- https://youtu.be/sX2nF1fW7kI
- https://youtu.be/8uAW5FQtcvE