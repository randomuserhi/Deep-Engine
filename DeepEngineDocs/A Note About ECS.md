The `ECDB` framework provided can be used to implement ECS, however it is important to note that you should not restrict yourself to such a general system if your use case is simple. For example ECS can be written manually for simple setups very easily:

Imagine defining a bunch of components:

```cpp
struct RigidBody {
    Deep::Vec3 position;
    Deep::Quat rotation;
    Deep::Vec3 velocity;
    Deep::Vec3 angularVelocity;
};

struct PlayerData {
    float health;
};

struct BleedData {
    float damage;
};
```

Then just manually create the required archetypes:

```cpp
class PlayerArchetype {
    RigidBody* rb;
    size_t size;
};

class TaggedPlayerArchetype {
    RigidBody* rb;
    size_t size;
};

class BleedingPlayerArchetype {
    RigidBody* rb;
    BleedData* bleed;
    size_t size;
};
```

Archetypes need code for managing adding/removing entities as well as moving entities between archetypes. This can easily be achieved through a set of general purpose functions such as:

```cpp
// NOTE:: More sophisticated templated generic functions can be used here as well

void removeComponent<T>(int index, T* componentList, size_t listSize) {
	// remove component and swap last item to fill gap...
} 

T* addComponent<T>(T* componentList, size_t listSize) {
	// resize component list to add an item and return pointer to last slot...
}

void memcpyComponent<T>(T* source, T* destination) {
	// memcpy the component data from the source pointer to the destination pointer
}

PlayerArchetype playerArch;
BleedingPlayerArchetype bleedingPlayerArch;

// playerArch -> bleedingPlayerArch

// Transfer rb component
memcpyComponent<RigidBody>(&playerArch.rb[5], addComponent<RigidBody>(bleedingPlayerArch.rb, bleedingPlayerArch.size));

// Remove rb component from original archetype
removeComponent<RigidBody>(5, playerArch.rb, playerArch.size);
--playerArch.size;

// Create bleeddata
BleedData* bleed = addComponent<BleedData>(bleedingPlayerArch.bleed, bleedingPlayerArch.size);
bleed->damage = 5.0f;

++bleedingPlayerArch.size;
```

Its not pretty, but showcases that you can definitely get far by manually coding compile-time ECS and following a data-oriented style.

Additionally, this approach can easily be done using metaprogramming where u can write code to generate the above ECS code. This way you do not need to wrangle with the template system.