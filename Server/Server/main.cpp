#include <iostream>

#include <Deep.h>
#include <Deep/Entity.h>
#include <Deep/Net.h>
#include <Deep/Math.h>
#include <Deep/Threading.h>

// https://stackoverflow.com/a/62047818/9642458
#include <chrono>
#include <thread>

#include <random>

struct RigidBody {
    Deep::Vec3 position;
    Deep::Vec3 velocity;
};

int main() {
    Deep::InitializeSockets();
    Deep::UDPSocket socket;
    socket.Open();
    int32 res = socket.Connect(Deep::IPv4(127, 0, 0, 1, 23152));
    Deep::IPv4 address;
    socket.GetPeerName(address);
    std::cout << static_cast<uint32>(address.a)        //
              << ":" << static_cast<uint32>(address.b) //
              << ":" << static_cast<uint32>(address.c) //
              << ":" << static_cast<uint32>(address.d) //
              << ":" << address.port << "\n";

    Deep::ECRegistry _registry{};
    Deep::ECDB database{ &_registry };
    Deep::ECStaticRegistry registry{ &_registry };

    Deep::ComponentId comp_RigidBody = registry.RegisterComponent<RigidBody>();
    Deep::ECDB::Archetype& arch_RigidBody = database.GetArchetype(&comp_RigidBody, 1);
    Deep::ECDB::Archetype::ComponentOffset offset_RigidBody = arch_RigidBody.GetComponentOffset(comp_RigidBody);

    for (size_t i = 0; i < 10; ++i) {
        arch_RigidBody.Entity();
    }

    for (Deep::ECDB::Archetype::Chunk* c = arch_RigidBody.chunks(); c != nullptr; c = arch_RigidBody.GetNextChunk(c)) {
        RigidBody* comps = arch_RigidBody.GetCompList<RigidBody>(c, offset_RigidBody);
        for (size_t i = 0; i < arch_RigidBody.GetChunkSize(c); ++i) {
            RigidBody& rb = comps[i];
            rb.position = { 1, 1, 1 };
            rb.velocity = { 1, 1, 1 };
        }
    }

    while (true) {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(50ms);
    };

    socket.Close();
    Deep::ShutdownSockets();
}