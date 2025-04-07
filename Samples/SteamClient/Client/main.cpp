#include <iostream>

#define DEEP_ENABLE_ASSERTS

#include <Deep.h>
#include <Deep/Memory.h>
#include <Deep/Math.h>
#include <Deep/Time.h>
#include <Deep/Net.h>

#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingsockets.h>

// https://stackoverflow.com/a/62047818/9642458
#include <chrono>
#include <thread>
#include <vector>

class Net {
public:
    Net(const SteamNetworkingIPAddr& serverAddr) {
        Deep_Assert(Net::instance == nullptr, "Only one instance of net should exist.");
        Net::instance = this;

        interface = SteamNetworkingSockets();
        Deep_Assert(interface != nullptr, "Should not happen.");

        SteamNetworkingConfigValue_t opt;
        opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged,
                   reinterpret_cast<void*>(SteamNetConnectionStatusChangedCallback));

        // NOTE(randomuserhi): IP connection just to dev on -> can use Relay later
        connection = interface->ConnectByIPAddress(serverAddr, 1, &opt);
        Deep_Assert(connection != k_HSteamNetConnection_Invalid, "Failed to start client.");
    }

    ~Net() {
        interface->CloseConnection(connection, 0, "Client Closed.", false);
        connection = k_HSteamNetConnection_Invalid;
    }

    void Send(const uint8* data, size_t numBytes) {
        interface->SendMessageToConnection(connection, data, numBytes, k_nSteamNetworkingSend_Reliable, nullptr);
    }

    void Tick() {
        interface->RunCallbacks();

        // Recieve messages:
        int numMsgs = interface->ReceiveMessagesOnConnection(connection, messageBuffer, messageBufferSize);
        if (numMsgs == 0) return;
        if (numMsgs < 0) return; // TODO(randomuserhi): Error state, should log

        for (size_t i = 0; i < numMsgs; ++i) {
            Deep::PacketReader packet(reinterpret_cast<uint8*>(messageBuffer[i]->m_pData));

            // TODO(randomuserhi): Handle recieved packet, parse it bla bla
            std::cout << "Recieved " << messageBuffer[i]->m_cbSize << "bytes: [" << packet.ReadUInt16() << "] "
                      << packet.head << "\n";

            messageBuffer[i]->Release();
        }
    }

private:
    ISteamNetworkingSockets* interface;
    HSteamNetConnection connection;

    static const size_t messageBufferSize = 10;
    ISteamNetworkingMessage* messageBuffer[messageBufferSize];

    void OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pInfo) {
        switch (pInfo->m_info.m_eState) {
        case k_ESteamNetworkingConnectionState_None:
            break;

        case k_ESteamNetworkingConnectionState_ClosedByPeer:
        case k_ESteamNetworkingConnectionState_ProblemDetectedLocally: {
            // Types of disconnect
            if (pInfo->m_eOldState == k_ESteamNetworkingConnectionState_Connecting) {
                // NOTE(randomuserhi): Could distinguish between a timeout, a rejected connection,
                //                     or some other transport problem.
                std::cout << "Unable to connect: " << pInfo->m_info.m_szEndDebug << "\n";
            } else if (pInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally) {
                std::cout << "Lost connection with host: " << pInfo->m_info.m_szEndDebug << "\n";
            } else {
                // NOTE(randomuserhi): Could check the reason code for a normal disconnection
                std::cout << "Host disconnect" << pInfo->m_info.m_szEndDebug << "\n";
            }

            // Clean up the connection.
            interface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
            connection = k_HSteamNetConnection_Invalid;
            break;
        }

        case k_ESteamNetworkingConnectionState_Connecting:
            break;

        case k_ESteamNetworkingConnectionState_Connected:
            std::cout << "Connected\n";
            break;

        default:
            // Silences -Wswitch
            break;
        }
    }

    static Net* instance;
    static void SteamNetConnectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t* pInfo) {
        Deep_Assert(instance != nullptr, "Should not happen.");

        instance->OnSteamNetConnectionStatusChanged(pInfo);
    }
};
Net* Net::instance = nullptr;

static Deep_Inline void Tick(Net& net, float32 dt) {
    Deep::PacketWriter packet;
    packet.Write((uint16)8);
    packet.Write(reinterpret_cast<const uint8*>("client!"), 8);
    net.Send(packet.data(), packet.size());
}

static void DebugOutput(ESteamNetworkingSocketsDebugOutputType eType, const char* pszMsg) {
    std::cout << pszMsg << "\n";
}

int main() {
    SteamDatagramErrMsg errMsg;
    if (!GameNetworkingSockets_Init(nullptr, errMsg)) {
        std::cout << "Failed to initialize steam api: " << errMsg << "\n";
        return 1;
    }

    SteamNetworkingIPAddr address;
    // address.ParseString("127.0.0.1:25617"); // NOTE(randomuserhi): Unresolved external symbol???
    address.SetIPv6LocalHost(25617);
    Net net{ address };

    // Main Tick Loop
    constexpr const int32 tickRate = 60; // Server runs at 60 ticks per second
    constexpr const int32 totalFrameTime = 1000 / tickRate;
    long long start = Deep::Epoch();
    while (true) {
        long long now = Deep::Epoch();
        float32 dt = static_cast<float>(now - start) / 1000.0f;
        start = now;

        // Network tick
        net.Tick();

        // Perform tick
        Tick(net, dt);

        long long end = Deep::Epoch();
        long long elapsed = end - start;

        // Spin lock to wait until next frame
        using namespace std::chrono_literals;
        long long timeleft = totalFrameTime - elapsed;
        if (timeleft < 0) {
            std::cout << "Tick took longer than expected! [" << elapsed << "ms]\n";
            timeleft = 0ll;
        }
        while (Deep::Epoch() < end + timeleft) {
            // std::this_thread::sleep_for(std::chrono::microseconds{ 1 });
        }
    };

    GameNetworkingSockets_Kill();

    return 0;
}