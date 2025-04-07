using Deep.Net;
using Steamworks;
using System;
using System.Text;
using UnityEngine;

internal class SteamTest : MonoBehaviour {
    private rSteamClient client;

    private void Start() {
        rSteamwork.Init();

        SteamNetworkingIPAddr address = new SteamNetworkingIPAddr();
        address.Clear();
        address.SetIPv6LocalHost(25617);
        client = new rSteamClient(address);
        client.onAccept += OnAccept;
        client.onReceive += OnReceive;
        client.onFail += OnFail;
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

    private void OnApplicationQuit() {
        client?.Dispose();
        client = null;
    }
}
