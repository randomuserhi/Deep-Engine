using System.Collections.Generic;
using UnityEngine;

public class Local : MonoBehaviour {
    public GameObject prefab;
    private List<SmoothTransform> objects = new List<SmoothTransform>();
    private List<Vector3> positions = new List<Vector3>();
    private List<Vector3> velocities = new List<Vector3>();

    // Start is called before the first frame update
    void Start() {
        while (objects.Count < 4000) {
            objects.Add(Instantiate(prefab).GetComponent<SmoothTransform>());
        }

        while (positions.Count < 2000000) {
            positions.Add(new Vector3(Random.Range(-1.0f, 1.0f), Random.Range(-1.0f, 1.0f), Random.Range(-1.0f, 1.0f)));
            velocities.Add(new Vector3(Random.Range(-1.0f, 1.0f), Random.Range(-1.0f, 1.0f), Random.Range(-1.0f, 1.0f)));
        }
    }

    // Update is called once per frame
    void FixedUpdate() {
        for (int i = 0; i < positions.Count; i++) {
            positions[i] += velocities[i];
        }

        for (int i = 0; i < objects.Count; i++) {
            objects[i].targetPosition = positions[i];
        }
    }
}
