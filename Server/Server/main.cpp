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
    std::random_device rd;  // Seed generator
    std::mt19937 gen(rd()); // Mersenne Twister RNG
    std::uniform_real_distribution<float> rand(-1.0f, 1.0f);

    Deep::JobSystem jobSystem{ 8, 2048, 1024 };

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
    Deep::ECDB database{ &_registry, 16384 * 8 };
    Deep::ECStaticRegistry registry{ &_registry };

    Deep::ComponentId comp_RigidBody = registry.RegisterComponent<RigidBody>();
    Deep::ECDB::Archetype& arch_RigidBody = database.GetArchetype(&comp_RigidBody, 1);
    Deep::ECDB::Archetype::ComponentOffset offset_RigidBody = arch_RigidBody.GetComponentOffset(comp_RigidBody);

    size_t numObjects = 1000000;
    for (size_t i = 0; i < numObjects; ++i) {
        arch_RigidBody.Entity();
    }

    for (Deep::ECDB::Archetype::Chunk* c = arch_RigidBody.chunks(); c != nullptr; c = arch_RigidBody.GetNextChunk(c)) {
        RigidBody* comps = arch_RigidBody.GetCompList<RigidBody>(c, offset_RigidBody);
        for (size_t i = 0; i < arch_RigidBody.GetChunkSize(c); ++i) {
            RigidBody& rb = comps[i];
            rb.position = { rand(gen), rand(gen), rand(gen) };
            rb.velocity = { rand(gen), rand(gen), rand(gen) };
        }
    }

    while (true) {
        {
            auto start = std::chrono::system_clock::now();

            Deep::Barrier barrier = jobSystem.AcquireBarrier();
            for (Deep::ECDB::Archetype::Chunk* c = arch_RigidBody.chunks(); c != nullptr;
                 c = arch_RigidBody.GetNextChunk(c)) {
                jobSystem.Enqueue([offset_RigidBody, c, &arch_RigidBody]() {
                    RigidBody* comps = arch_RigidBody.GetCompList<RigidBody>(c, offset_RigidBody);
                    for (size_t i = 0; i < arch_RigidBody.GetChunkSize(c); ++i) {
                        RigidBody& rb = comps[i];
                        rb.position += rb.velocity;
                    }
                });
            }
            barrier.Wait();

            auto end = std::chrono::system_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            std::cout << elapsed.count() << "ms frame time - ";
        }

        {
            auto start = std::chrono::system_clock::now();
            Deep::PacketWriter packet;

            // packet.Write(static_cast<int32>(arch_RigidBody.size()));
            packet.Write(4000);
            size_t count = 0;
            for (Deep::ECDB::Archetype::Chunk* c = arch_RigidBody.chunks(); count < 4000 && c != nullptr;
                 c = arch_RigidBody.GetNextChunk(c)) {
                RigidBody* comps = arch_RigidBody.GetCompList<RigidBody>(c, offset_RigidBody);
                for (size_t i = 0; count < 4000 && i < arch_RigidBody.GetChunkSize(c); ++i) {
                    RigidBody& rb = comps[i];
                    packet.WriteHalf(rb.position);
                    ++count;
                }
            }

            socket.Send(packet);

            auto end = std::chrono::system_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            std::cout << elapsed.count() << "ms net time\n";
        }

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(100ms);
    };

    socket.Close();
    Deep::ShutdownSockets();
}