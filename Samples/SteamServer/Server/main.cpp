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
private:
    struct Client {
        Client(HSteamNetConnection connection) :
            connection(connection) {}

        HSteamNetConnection connection;
    };

public:
    Net(uint16 port) {
        Deep_Assert(Net::instance == nullptr, "Only one instance of net should exist.");
        Net::instance = this;

        interface = SteamNetworkingSockets();
        Deep_Assert(interface != nullptr, "Should not happen.");

        SteamNetworkingIPAddr serverLocalAddr;
        serverLocalAddr.Clear();
        serverLocalAddr.m_port = port;

        SteamNetworkingConfigValue_t opt;
        opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged,
                   reinterpret_cast<void*>(SteamNetConnectionStatusChangedCallback));

        // NOTE(randomuserhi): IP connection just to dev on -> can use Relay later
        listenSocket = interface->CreateListenSocketIP(serverLocalAddr, 1, &opt);
        Deep_Assert(listenSocket != k_HSteamListenSocket_Invalid, "Failed to start listen socket.");

        pollGroup = interface->CreatePollGroup();
        Deep_Assert(pollGroup != k_HSteamNetPollGroup_Invalid, "Failed to create poll group.");
    }

    ~Net() {
        for (Client& client : clients) {
            interface->CloseConnection(client.connection, 0, "Server Shutdown.", false);
        }
        clients.clear();

        interface->CloseListenSocket(listenSocket);
        listenSocket = k_HSteamListenSocket_Invalid;

        interface->DestroyPollGroup(pollGroup);
        pollGroup = k_HSteamNetPollGroup_Invalid;
    }

    void Send(const uint8* data, size_t numBytes) {
        for (Client& client : clients) {
            SendTo(client.connection, data, numBytes);
        }
    }

    Deep_Inline void SendTo(HSteamNetConnection connection, const uint8* data, size_t numBytes) {
        interface->SendMessageToConnection(connection, data, numBytes, k_nSteamNetworkingSend_Reliable, nullptr);
    }

    void Tick() {
        interface->RunCallbacks();

        // Recieve messages:
        int numMsgs = interface->ReceiveMessagesOnPollGroup(pollGroup, messageBuffer, messageBufferSize);
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
    HSteamListenSocket listenSocket;
    HSteamNetPollGroup pollGroup;

    static const size_t messageBufferSize = 10;
    ISteamNetworkingMessage* messageBuffer[messageBufferSize];

    std::vector<Client> clients;

    void OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pInfo) {
        switch (pInfo->m_info.m_eState) {
        case k_ESteamNetworkingConnectionState_None:
            break;

        case k_ESteamNetworkingConnectionState_ClosedByPeer:
        case k_ESteamNetworkingConnectionState_ProblemDetectedLocally: {
            // Ignore if they were not previously connected.  (If they disconnected
            // before we accepted the connection.)
            if (pInfo->m_eOldState == k_ESteamNetworkingConnectionState_Connected) {

                // NOTE(randomuserhi): Not many clients, so linear iteration should be fine.
                Client* client = nullptr;
                size_t i = 0;
                for (; i < clients.size(); ++i) {
                    if (clients[i].connection == pInfo->m_hConn) {
                        client = &(clients[i]);
                        break;
                    }
                }
                Deep_Assert(client != nullptr, "Should not happen.");

                std::cout << "Connection from " << pInfo->m_hConn << " has closed.\n";

                // TODO(randomuserhi): Log something when client disconnects... Callbacks etc...

                clients.erase(clients.begin() + i);
            } else {
                Deep_Assert(pInfo->m_eOldState == k_ESteamNetworkingConnectionState_Connecting, "Should not happen.");
            }

            interface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
            break;
        }

        case k_ESteamNetworkingConnectionState_Connecting: {
#ifdef DEEP_ENABLE_ASSERTS
            // This must be a new connection
            Client* client = nullptr;
            for (Client& c : clients) {
                if (c.connection == pInfo->m_hConn) {
                    client = &c;
                    break;
                }
            }
            Deep_Assert(client == nullptr, "Should not happen.");
#endif

            std::cout << "Connection request from " << pInfo->m_info.m_szConnectionDescription << "\n";

            // A client is attempting to connect
            // Try to accept the connection.
            if (interface->AcceptConnection(pInfo->m_hConn) != k_EResultOK) {
                // This could fail.  If the remote host tried to connect, but then
                // disconnected, the connection may already be half closed.  Just
                // destroy whatever we have on our side.
                interface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
                break;
            }

            // Assign the poll group
            if (!interface->SetConnectionPollGroup(pInfo->m_hConn, pollGroup)) {
                // Failed to set poll group, just fail
                interface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
                break;
            }

            // Add them to the client list
            clients.emplace_back(pInfo->m_hConn);
            break;
        }

        case k_ESteamNetworkingConnectionState_Connected:
            // We will get a callback immediately after accepting the connection.
            // Since we are the server, we can ignore this, it's not news to us.
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
    packet.Write(reinterpret_cast<const uint8*>("server!"), 8);
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

    Net net{ 25617 };

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