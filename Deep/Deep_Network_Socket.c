#include "Deep_Network.h"

#include <stdio.h>
#include <stdint.h>

const struct Deep_Network_Address Deep_Network_Address_Default = { .a = 127, .b = 0, .c = 0, .d = 1, .port = 0 };
const struct Deep_Network_Socket Deep_Network_Socket_Default = { INVALID_SOCKET, DEEP_NETWORK_DEFAULTPORT };

#if defined(DEEP_PLATFORM_WINDOWS)

BOOL Deep_Network_IsBigEndian() //https://stackoverflow.com/questions/6136010/is-using-an-union-in-place-of-a-cast-well-defined
{
	const uint32_t i = 1;
	return i == htonl(i);
}

BOOL Deep_Network_InitializeSockets()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) == NO_ERROR)
	{
		printf("WSA Initialized.\n");
		return TRUE;
	}
	printf("WSA failed to initialize with error = %i\n", WSAGetLastError());
	return FALSE;
}

void Deep_Network_ShutdownSockets()
{
	const int result = WSACleanup();
	if (result == NOERROR)
		printf("WSA successfully cleaned up.\n");
	else
		printf("WSA failed to cleaup with error = %i.\n", WSAGetLastError());
}

void Deep_Network_Address_Format(struct Deep_Network_Address* address)
{
	const unsigned int BitAddress = (address->a << 24) | (address->b << 16) | (address->c << 8) | address->d;

	address->sockAddr.sa_in.sin_family = AF_INET;
	address->sockAddr.sa_in.sin_addr.s_addr = htonl(BitAddress);
	address->sockAddr.sa_in.sin_port = htons(address->port);
}

BOOL Deep_Network_Address_Equal(const struct Deep_Network_Address* lhs, const struct Deep_Network_Address* rhs)
{
	return (lhs->sockAddr.sa_in.sin_addr.s_addr == rhs->sockAddr.sa_in.sin_addr.s_addr) && (lhs->sockAddr.sa_in.sin_port == rhs->sockAddr.sa_in.sin_port);
}

unsigned short Deep_Network_socket_GetPort(const struct Deep_Network_Socket* deepSocket)
{
	return deepSocket->port;
}

int Deep_Network_Socket_Open(struct Deep_Network_Socket* deepSocket)
{
	deepSocket->socketFD = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (deepSocket->socketFD == INVALID_SOCKET)
	{
		const int err = WSAGetLastError();
		printf("Socket function failed with error = %i.\n", err);
		return err;
	}
	printf("Socket function succeeded.\n");

	//Need to be aware of https://stackoverflow.com/questions/34242622/windows-udp-sockets-recvfrom-fails-with-error-10054
	#define SIO_UDP_CONNRESET _WSAIOW(IOC_VENDOR, 12)
	BOOL bNewBehavior = FALSE;
	DWORD dwBytesReturned = 0;
	WSAIoctl(deepSocket->socketFD, SIO_UDP_CONNRESET, &bNewBehavior, sizeof bNewBehavior, NULL, 0, &dwBytesReturned, NULL, NULL);

	return DEEP_NETWORK_NOERROR;
}
int Deep_Network_Socket_Close(struct Deep_Network_Socket* deepSocket)
{
	if (deepSocket->socketFD == INVALID_SOCKET)
	{
		printf("Socket already closed / SocketFD is invalid.\n");
		return DEEP_NETWORK_NOERROR;
	}

	const int result = closesocket(deepSocket->socketFD);
	if (result == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		printf("Failed to close socket with error = %i.\n", err);
		return err;
	}
	printf("Closed socket successfully.\n");
	deepSocket->socketFD = INVALID_SOCKET;
	return DEEP_NETWORK_NOERROR;
}

int Deep_Network_Socket_Bind(struct Deep_Network_Socket* deepSocket, unsigned short port)
{
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	if (bind(deepSocket->socketFD, (struct sockaddr*)&address, sizeof address) == SOCKET_ERROR)
	{
		const int err = WSAGetLastError();
		printf("Socket failed to bind with error %i.\n", err);
		return err;
	}
	else
	{
		printf("Socket bound successfully.\n");

		union Deep_SocketAddr assignedAddress;
		socklen_t assignedAddressLen = sizeof assignedAddress;

		//Get assigned port
		if (getsockname(deepSocket->socketFD, &assignedAddress.sa, &assignedAddressLen) != NO_ERROR)
			printf("Failed to get socket details.\n");
		else
		{
			deepSocket->port = ntohs(assignedAddress.sa_in.sin_port);
			printf("Socket bound to port %i.\n", deepSocket->port);
		}

		//Set socket to be non-blocking

		DWORD NonBlocking = 1;
		if (ioctlsocket(deepSocket->socketFD, FIONBIO, &NonBlocking) != 0)
		{
			printf("Failed to set socket to non-blocking.\n");
			return DEEP_NETWORK_ERROR;
		}

		printf("Successfully set socket to non-blocking.\n");

		return DEEP_NETWORK_NOERROR;
	}
}

int Deep_Network_Socket_GetRemainingBytes(struct Deep_Network_Socket* deepSocket)
{
	u_long remainingBytes;
	const int result = ioctlsocket(deepSocket->socketFD, FIONREAD, &remainingBytes);
	if (result != NOERROR)
	{
		printf("Failed to get remaining bytes with error = %i.\n", WSAGetLastError());
		return DEEP_NETWORK_ERROR;
	}
	return remainingBytes;
}

int Deep_Network_Socket_Send(struct Deep_Network_Socket* deepSocket, const char* data, int dataSize, union Deep_SocketAddr* address)
{
	const int SentBytes = sendto(deepSocket->socketFD, data, dataSize, 0, &address->sa, sizeof *address);
	if (SentBytes != dataSize)
	{
		const int err = WSAGetLastError();
		printf("Failed to send packet to destination with error = %i.\n", err);
		return err;
	}
	//printf("Packet successfully sent.\n");
	return DEEP_NETWORK_NOERROR;
}

int Deep_Network_Socket_Receive(struct Deep_Network_Socket* deepSocket, char* buffer, int maxBufferSize, union Deep_SocketAddr* fromAddress)
{
	socklen_t fromLength = sizeof (struct sockaddr_in);

	const int BytesReceived = recvfrom(deepSocket->socketFD, (char*)buffer, maxBufferSize, 0, &fromAddress->sa, &fromLength);

	if (BytesReceived < 0)
	{
		const int Error = WSAGetLastError();
		if (Error != WSAEWOULDBLOCK) //https://stackoverflow.com/questions/17064069/recvfrom-error-10035-using-non-blocking-sockets
		{
			printf("Failed to receive packet with error = %i.\n", Error);
			return DEEP_NETWORK_ERROR;
		}
		else
			return DEEP_NETWORK_ERROR;
	}

	return BytesReceived;
}

#endif
