*Maybe rename to DeepFramework*

Deep Engine aims to be a framework for making game engines instead of a game engine itself:
- Follows the same design philosophy and [Psyshock](https://github.com/Dreaming381/Latios-Framework-Documentation/blob/main/Psyshock%20Physics/README.md) in which it offers low level access to the bare minimum and you create the pipeline yourself.
- This is the "choose your own toppings" solution in which the developer makes their own "pizza" over choosing a premade one.
	- `ECS` is not a full ECS system but instead is only the Entity Component Database supporting queries, referencing and storing but not systems at all. The developer instead uses the database operations and can create their own custom systems solution with their own pipeline. 
	- `Physics` is not a full physics engine but instead offers just math collision operations and solvers (as functions / premade systems) that you can hook up yourself to make a full physics simulation.
		- This removes the need to "hack" physics simulations to create your own handmade solutions for specific scenarios. Specifically character controllers.
			- Unity rigid body based character controllers are the bane of anyone's existence.
