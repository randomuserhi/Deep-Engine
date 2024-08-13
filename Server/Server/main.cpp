#include <iostream>

#include "Deep.h"
#include "Deep/Net.h"
#include "Deep/Math.h"

// https://stackoverflow.com/a/62047818/9642458
#include <chrono>
#include <thread>

// TODO(randomuserhi): Documentation, I need to stay on top of documentation
//                     in code docs along side doc website

int main() {
    std::cout << "Hello World!" << std::endl;
    Deep::InitializeSockets();
    Deep::UDPSocket socket;
    socket.Open();
    int32 res = socket.Connect(Deep::IPv4(127, 0, 0, 1, 23152));
    Deep::IPv4 address;
    socket.GetPeerName(address);
    std::cout << static_cast<uint32>(address.a)
        << ":" << static_cast<uint32>(address.b)
        << ":" << static_cast<uint32>(address.c)
        << ":" << static_cast<uint32>(address.d)
        << ":" << address.port << std::endl;

    const uint8 data[] = "That's crazy";

    std::cout << sizeof data << std::endl;

    Deep::Vec3 up{ 0, 1, 0 };
    Deep::Vec3 axis{ 0, 0, 1 };
    Deep::Quat rotateLeft{ axis, PI / 2.0f };
    Deep::Vec3 right = rotateLeft * up;
    std::cout << rotateLeft.w
        << "," << rotateLeft.x
        << "," << rotateLeft.y
        << "," << rotateLeft.z
        << std::endl;
    std::cout << right.x
        << "," << right.y
        << "," << right.z
        << std::endl;

    std::cout << std::endl;

    while (true) {
        Deep::PacketWriter packet;
        packet.Write(static_cast<uint16>(sizeof data));
        packet.Write(data, sizeof data);

        socket.Send(packet);

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(100ms);
    };

    socket.Close();
    Deep::ShutdownSockets();
}