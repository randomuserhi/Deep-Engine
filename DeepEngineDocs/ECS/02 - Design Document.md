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