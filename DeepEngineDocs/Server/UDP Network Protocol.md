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
	- [Building](https://www.youtube.com/watch?v=jS9rBienEFQ)
	- Alternatively, follow the github BUILDING readme, but make sure to not have msys2 bound in system environment variables as it fucks up the VS developer console.
	- When linking, remember to set compile definition  for `STEAMNETWORKINGSOCKETS_STATIC_LINK` [ref](https://stackoverflow.com/questions/70714178/linking-with-gamenetworkingsockets-library-on-windows-gives-unresolved-external)
- Steamworks API (Steam Relay)
	- [ref](https://stackoverflow.com/a/75692299)