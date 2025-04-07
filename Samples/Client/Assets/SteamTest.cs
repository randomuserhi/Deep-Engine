using Deep.Net;
using Steamworks;
using System;
using System.Text;
using UnityEngine;

internal class SteamTest : MonoBehaviour {
    private rSteamClient client;
    private rSteamServer server;

    private void Start() {
        if (!SteamManager.Initialized) {
            Debug.LogError("No steamworks.");
            return;
        }

        // TODO(randomuserhi): Should move elsewhere
        if (!GameServer.Init(0x7f000001, 0, 0, EServerMode.eServerModeNoAuthentication, "0.0.0.1")) {
            Debug.LogError("No game server.");
            return;
        }

        /*// Wait for relay network access
        SteamNetworkingUtils.InitRelayNetworkAccess();
        SteamRelayNetworkStatus_t status;
        ESteamNetworkingAvailability eAvailable;
        while ((eAvailable = SteamNetworkingUtils.GetRelayNetworkStatus(out status)) != ESteamNetworkingAvailability.k_ESteamNetworkingAvailability_Current) {
            SteamAPI.RunCallbacks(); // NOTE(randomuserhi): Important otherwise steam can't manage the connection status.
            System.Threading.Thread.Sleep(100);
        }
        Debug.Log("Connected to Steam Relay!");*/

        server = new rSteamServer(11111);

        /*client = new rSteamClient(76561198815023875, 11111);
        client.onAccept += OnAccept;
        client.onReceive += OnReceive;
        client.onFail += OnFail;*/
    }

    private void OnFail(rSteamClient client) {
        Debug.Log("Failed!");
    }

    private void OnAccept(rSteamClient client) {
        Debug.Log("Connected!");
        send = true;
    }

    private void OnReceive(ArraySegment<byte> buffer, rSteamClient client) {
        Debug.Log($"Received {buffer.Count} bytes: {Encoding.ASCII.GetString(new ReadOnlySpan<byte>(buffer.Array!, buffer.Offset + sizeof(ushort), BitHelper.ReadShort(buffer, 0)))}");
    }

    private bool send = false;
    private void Update() {
        if (!send) return;

        byte[] test = Encoding.ASCII.GetBytes("client");
        ByteBuffer packet = new ByteBuffer();
        BitHelper.WriteBytes(test, packet);
        client.Send(packet.Array);
    }

    // TODO(randomuserhi): Should move elsewhere
    private void OnDestroy() {
        GameServer.Shutdown();
    }

    private void OnApplicationQuit() {
        client?.Dispose();
        client = null;

        server?.Dispose();
        server = null;
    }
}
