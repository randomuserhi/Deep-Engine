#ifndef h_Deep_Network_Windows
#define h_Deep_Network_Windows

#include <winsock2.h>

typedef int socklen_t;

union Deep_SocketAddr
{
	struct sockaddr sa;
	struct sockaddr_in sa_in;
};

BOOL Deep_Network_InitializeSockets();
void Deep_Network_ShutdownSockets();

BOOL Deep_Network_IsBigEndian();

struct Deep_Network_Address
{
	unsigned char a;
	unsigned char b;
	unsigned char c;
	unsigned char d;
	unsigned short port;

	union Deep_SocketAddr sockAddr;
};
extern const struct Deep_Network_Address Deep_Network_Address_Default;

void Deep_Network_Address_Format(struct Deep_Network_Address* address);
BOOL Deep_Network_Address_Equal(const struct Deep_Network_Address* lhs, const struct Deep_Network_Address* rhs);

struct Deep_Network_Socket
{
	SOCKET socketFD;
	unsigned short port;
};
extern const struct Deep_Network_Socket Deep_Network_Socket_Default;

unsigned short Deep_Network_socket_GetPort(const struct Deep_Network_Socket* deepSocket);

int Deep_Network_Socket_Open(struct Deep_Network_Socket* deepSocket);
int Deep_Network_Socket_Close(struct Deep_Network_Socket* deepSocket);

int Deep_Network_Socket_Bind(struct Deep_Network_Socket* deepSocket, unsigned short port);

int Deep_Network_Socket_GetRemainingBytes(struct Deep_Network_Socket* deepSocket);

int Deep_Network_Socket_Send(struct Deep_Network_Socket* deepSocket, const char* data, int dataSize, union Deep_SocketAddr* address);
Deep_Inline int Deep_Network_Socket_Send_DeepAddress(struct Deep_Network_Socket* deepSocket, const char* data, int dataSize, struct Deep_Network_Address* address)
{
	return Deep_Network_Socket_Send(deepSocket, data, dataSize, &address->sockAddr);
}

int Deep_Network_Socket_Receive(struct Deep_Network_Socket* deepSocket, char* buffer, int maxBufferSize, union Deep_SocketAddr* fromAddress);

struct Deep_Network_Server
{
	struct Deep_Network_Socket socket;
	union Deep_SocketAddr from;

	char* buffer;
	unsigned int maxBufferSize;

	void (*OnReceiveHandle)(const char*, int, unsigned int, unsigned int);
};
extern const struct Deep_Network_Server Deep_Network_Server_Default;

void Deep_Network_Server_Start(struct Deep_Network_Server* deepServer, unsigned short port);
void Deep_Network_Server_Close(struct Deep_Network_Server* deepServer);
void Deep_Network_Server_Tick(struct Deep_Network_Server* deepServer);

Deep_Inline int Deep_Network_Server_Send(struct Deep_Network_Server* deepServer, const char* Data, int DataSize, union Deep_SocketAddr* Address)
{
	return Deep_Network_Socket_Send(&deepServer->socket, Data, DataSize, Address);
}

#endif
