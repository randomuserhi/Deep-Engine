using UnityEngine;

public class SmoothTransform : MonoBehaviour {
    public Vector3 targetPosition;

    // Start is called before the first frame update
    void Start() {

    }

    // Update is called once per frame
    void Update() {
        transform.position = Vector3.Lerp(transform.position, targetPosition, 10 * Time.deltaTime);
    }
}
