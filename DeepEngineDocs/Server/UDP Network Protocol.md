https://gafferongames.com/post/reliability_ordering_and_congestion_avoidance_over_udp/

- A connection is defined as a constant stream of packets between client and server.
	- Client sends `keep alive` packets to the server once a second.
	
- Communication tick rate is varied (not fixed)
	- Keep a moving average of incoming packet rate - this is used to determine when a packet has dropped (knowing the rate of like 20 packets a second, if you haven't seen the ack for a packet in 1 second, its probably dropped etc...)
	- API to set the rate as well instead of automatically detecting the rate?

```
sequence number: uint16
remote sequence: uint16 (last remote sequence number recieved)
ackBitField: uint32
<data>
```

Wrap around:
```cpp
inline bool sequence_greater_than( uint16_t s1, uint16_t s2 ) { 
	return ( ( s1 > s2 ) && ( s1 - s2 <= 32768 ) ) || 
	       ( ( s1 < s2 ) && ( s2 - s1 > 32768 ) ); 
}
```

#### 3 Net examples
- Handmade (the above)
- [GameNetworkingSockets](https://github.com/ValveSoftware/GameNetworkingSockets/)
- Steamworks API (Steam Relay)