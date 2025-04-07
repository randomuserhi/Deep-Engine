using Steamworks;
using UnityEngine;

internal static class rSteamwork {
    public static bool initialized = false;
    public static void Init() {
        if (initialized) return;
        initialized = true;

        if (!SteamAPI.Init()) {
            Debug.LogError("Failed to start SteamAPI.");
            return;
        }

        /*AppId_t app = new AppId_t() { m_AppId = 480 };
        if (SteamAPI.RestartAppIfNecessary(app)) {
            Debug.LogError("Failed to set appID.");
            return;
        }*/

        if (!GameServer.Init(0x7f000001, 0, 0, EServerMode.eServerModeNoAuthentication, "0.0.0.1")) {
            Debug.LogError("Failed to start GameServer");
            return;
        }
        SteamNetworkingUtils.InitRelayNetworkAccess();

        /*SteamNetworkingUtils.SetDebugOutputFunction(ESteamNetworkingSocketsDebugOutputType.k_ESteamNetworkingSocketsDebugOutputType_Everything, (type, message) => {
            APILogger.Debug($"[STEAMWORKS] {type}: {message}");
        });*/

        /*unsafe {
            int bufferSize = 10 * 1024 * 1024;
            int* bufferSize_ptr = &bufferSize;
            if (!SteamNetworkingUtils.SetConfigValue(ESteamNetworkingConfigValue.k_ESteamNetworkingConfig_SendBufferSize, ESteamNetworkingConfigScope.k_ESteamNetworkingConfig_Global, IntPtr.Zero, ESteamNetworkingConfigDataType.k_ESteamNetworkingConfig_Int32, (IntPtr)bufferSize_ptr)) {
                APILogger.Warn("Failed to increase SendBufferSize!");
            }
        }*/
    }
}
