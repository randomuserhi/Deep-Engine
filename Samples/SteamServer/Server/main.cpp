#include <iostream>

#include <Deep.h>
#include <Deep/Memory.h>
#include <Deep/Math.h>
#include <Deep/Time.h>

#include <steam/steam_api.h>
#include <steam/isteamnetworkingsockets.h>

// https://stackoverflow.com/a/62047818/9642458
#include <chrono>
#include <thread>
#include <vector>

class Net {
    // TODO(randomuserhi):

    Net(uint16 port) {
        interface = SteamNetworkingSockets();

        SteamNetworkingIPAddr serverLocalAddr;
        serverLocalAddr.Clear();
        serverLocalAddr.m_port = port;

        SteamNetworkingConfigValue_t opt;
        opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged,
                   (void*)SteamNetConnectionStatusChangedCallback);

        /*listenSock = m_pInterface->CreateListenSocketIP(serverLocalAddr, 1, &opt);
        if (m_hListenSock == k_HSteamListenSocket_Invalid) FatalError("Failed to listen on port %d", nPort);
        pollGroup = m_pInterface->CreatePollGroup();
        if (m_hPollGroup == k_HSteamNetPollGroup_Invalid) FatalError("Failed to listen on port %d", nPort);
        Printf("Server listening on port %d\n", nPort);*/
    }

    ~Net() {}

private:
    ISteamNetworkingSockets* interface;

    void OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pInfo) {}

    static Net* instance;
    static void SteamNetConnectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t* pInfo) {
        instance->OnSteamNetConnectionStatusChanged(pInfo);
    }
};
Net* Net::instance = nullptr;

static Deep_Inline void NetTick(Net& net) {
    // TODO(randomuserhi):

    int32 bytesReceived;
    while (false) {
        std::cout << "Received " << bytesReceived << " bytes.\n";
    }
}

static Deep_Inline void Tick(Net& net, float32 dt) {}

int main() {
    SteamAPI_Init();

    // TODO(randomuserhi): Init netcode
    Net net;

    // Main Tick Loop
    constexpr const int32 tickRate = 60; // Server runs at 60 ticks per second
    constexpr const int32 totalFrameTime = 1000 / tickRate;
    long long start = Deep::Epoch();
    while (true) {
        long long now = Deep::Epoch();
        float32 dt = static_cast<float>(now - start) / 1000.0f;
        start = now;

        // Network tick
        NetTick(net);

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

    SteamAPI_Shutdown();

    return 0;
}