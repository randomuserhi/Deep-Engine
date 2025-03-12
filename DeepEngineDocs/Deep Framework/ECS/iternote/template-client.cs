using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using UnityEngine;

internal class Test : MonoBehaviour {
    private UdpClient client;

    private void Start() {
        client = new UdpClient("127.0.0.1", 23152);
        _ = Listen().ConfigureAwait(false);

        Debug.Log("started");
    }

    private async Task Listen() {
        UdpReceiveResult result = await client.ReceiveAsync();

        Debug.Log($"Received: {result.Buffer.Count()} bytes.");
        _ = Listen().ConfigureAwait(false);
    }

    private void Update() {
        byte[] test = Encoding.ASCII.GetBytes("client");
        client.Send(test, test.Length);
    }

    private void OnApplicationQuit() {
        client?.Dispose();
    }
}
