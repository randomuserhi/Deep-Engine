using Deep.Net;
using System.Net.Sockets;
using System.Threading.Tasks;
using UnityEngine;

internal class Test : MonoBehaviour {
    private UdpClient client;

    private void Start() {
        client = new UdpClient(23152);
        _ = Listen();

        Debug.Log("started");
    }

    private async Task Listen() {
        UdpReceiveResult result = await client.ReceiveAsync();
        int i = 0;
        Debug.Log(BitHelper.ReadString(result.Buffer, ref i));
        _ = Listen();
    }

    private void OnApplicationQuit() {
        client?.Dispose();
    }
}
