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
    socket.Connect(Deep::IPv4(127, 0, 0, 1, 1000));
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
    Deep::Quaternion rotateRight{ axis, PI / 2.0f };
    Deep::Vec3 right = rotateRight * up;
    std::cout << rotateRight.w
        << "," << rotateRight.x
        << "," << rotateRight.y
        << "," << rotateRight.z
        << std::endl;
    std::cout << right.x
        << "," << right.y
        << "," << right.z
        << std::endl;

    while (true) {
        Deep::PacketWriter packet;
        packet.Write(static_cast<uint8>(sizeof data));
        packet.Write(data, sizeof data);

        socket.Send(packet.data(), packet.size());

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(100ms);
    };

    socket.Close();
    Deep::ShutdownSockets();
}