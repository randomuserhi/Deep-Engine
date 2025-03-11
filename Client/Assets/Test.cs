using Deep.Net;
using System.Collections.Generic;
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

    public GameObject prefab;
    private List<SmoothTransform> objects = new List<SmoothTransform>();

    private async Task Listen() {
        UdpReceiveResult result = await client.ReceiveAsync();

        int _i = 0;
        int numObjects = BitHelper.ReadInt(result.Buffer, ref _i);

        while (objects.Count < numObjects) {
            objects.Add(Instantiate(prefab).GetComponent<SmoothTransform>());
        }

        for (int i = 0; i < numObjects; ++i) {
            objects[i].targetPosition = BitHelper.ReadHalfVector3(result.Buffer, ref _i);
        }

        _ = Listen();
    }

    private void OnApplicationQuit() {
        client?.Dispose();
    }
}
