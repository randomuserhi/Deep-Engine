#include "Deep_Network.h"
#include "Deep_Network_Windows.h"

#if defined(DEEP_PLATFORM_WINDOWS)

namespace Deep
{
	bool IsBigEndian() //https://stackoverflow.com/questions/6136010/is-using-an-union-in-place-of-a-cast-well-defined
	{
		const uint32_t i = 1;
		return i == htonl(i);
	}

	int InitializeSockets()
	{
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) == NO_ERROR)
		{
			std::cout << "WSA Initialized.\n";
			return DEEP_SOCKET_NOERROR;
		}
		std::cout << "WSA failed to initialize with error = " << WSAGetLastError() << '\n';
		return DEEP_SOCKET_ERROR;
	}

	void ShutdownSockets()
	{
		const int result = WSACleanup();
		if (result == NOERROR)
			std::cout << "WSA successfully cleaned up.\n";
		else
			std::cout << "WSA failed to cleaup with error = " << WSAGetLastError() << '\n';
	}

	namespace Impl
	{
		int Socket::Open()
		{
			socketFD = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

			if (socketFD == INVALID_SOCKET)
			{
				const int err = WSAGetLastError();
				std::cout << "Socket function failed with error = " << err << '\n';
				return DEEP_SOCKET_ERROR;
			}
			std::cout << "Socket function succeeded.\n";

			//Need to be aware of https://stackoverflow.com/questions/34242622/windows-udp-sockets-recvfrom-fails-with-error-10054
#define SIO_UDP_CONNRESET _WSAIOW(IOC_VENDOR, 12)
			BOOL bNewBehavior = FALSE;
			DWORD dwBytesReturned = 0;
			WSAIoctl(socketFD, SIO_UDP_CONNRESET, &bNewBehavior, sizeof bNewBehavior, NULL, 0, &dwBytesReturned, NULL, NULL);

			return DEEP_SOCKET_NOERROR;
		}

		int Socket::Close()
		{
			if (socketFD == INVALID_SOCKET)
			{
				std::cout << "Socket already closed / SocketFD is invalid.\n";
				return DEEP_SOCKET_NOERROR;
			}

			const int result = closesocket(socketFD);
			if (result == SOCKET_ERROR)
			{
				int err = WSAGetLastError();
				std::cout << "Failed to close socket with error = " << err << '\n';
				return DEEP_SOCKET_ERROR;
			}
			std::cout << "Closed socket successfully.\n";
			socketFD = INVALID_SOCKET;
			return DEEP_SOCKET_NOERROR;
		}

		int Socket::Bind(unsigned short port)
		{
			struct sockaddr_in address;
			address.sin_family = AF_INET;
			address.sin_addr.s_addr = INADDR_ANY;
			address.sin_port = htons(port);

			if (bind(socketFD, (struct sockaddr*)&address, sizeof address) == SOCKET_ERROR)
			{
				const int err = WSAGetLastError();
				std::cout << "Socket failed to bind with error " << err << '\n';
				return err;
			}
			else
			{
				std::cout << "Socket bound successfully.\n";

				SocketAddr assignedAddress;

				//Get assigned port
				if (GetSockName(assignedAddress) != DEEP_SOCKET_NOERROR)
					std::cout << "Failed to get socket details.\n";
				else
					std::cout << "Socket bound to port " << ntohs(assignedAddress.sa_in.sin_port) << '\n';

				//Set socket to be non-blocking

				DWORD nonBlocking = 1;
				if (ioctlsocket(socketFD, FIONBIO, &nonBlocking) != 0)
				{
					std::cout << "Failed to set socket to non-blocking.\n";
					return DEEP_SOCKET_ERROR;
				}

				std::cout << "Successfully set socket to non-blocking.\n";

				return DEEP_SOCKET_NOERROR;
			}
		}

		int Socket::Connect(const SocketAddr& address)
		{
			socklen_t addressLength = sizeof(struct sockaddr);
			if (connect(socketFD, &address.sa, addressLength) != NO_ERROR)
			{
				const int err = WSAGetLastError();
				std::cout << "Failed to connect socket with error = " << err << '\n';
				return DEEP_SOCKET_ERROR;
			}
			return DEEP_SOCKET_NOERROR;
		}

		int Socket::Send(const char* data, int dataSize, const SocketAddr& address)
		{
			const int sentBytes = send(socketFD, data, dataSize, 0);
			if (sentBytes == SOCKET_ERROR)
			{
				const int err = WSAGetLastError();
				std::cout << "Failed to send packet to destination with error = " << err << '\n';
				return DEEP_SOCKET_ERROR;
			}
			return DEEP_SOCKET_NOERROR;
		}

		int Socket::SendTo(const char* data, int dataSize, const SocketAddr& address)
		{
			const int sentBytes = sendto(socketFD, data, dataSize, 0, &address.sa, sizeof address);
			if (sentBytes == SOCKET_ERROR)
			{
				const int err = WSAGetLastError();
				std::cout << "Failed to send packet to destination with error = " << err << '\n';
				return DEEP_SOCKET_ERROR;
			}
			return DEEP_SOCKET_NOERROR;
		}

		int Socket::Receive(char* buffer, int maxBufferSize, int& bytesReceived, SocketAddr& fromAddress)
		{
			socklen_t fromLength = sizeof(struct sockaddr_in);

			bytesReceived = recvfrom(socketFD, buffer, maxBufferSize, 0, &fromAddress.sa, &fromLength);

			if (bytesReceived < 0)
			{
				const int err = WSAGetLastError();
				if (err != WSAEWOULDBLOCK) //https://stackoverflow.com/questions/17064069/recvfrom-error-10035-using-non-blocking-sockets
				{
					std::cout << "Failed to receive packet with error = " << err << '\n';
					return DEEP_SOCKET_ERROR;
				}
				else
					return DEEP_SOCKET_ERROR;
			}

			return DEEP_SOCKET_NOERROR;
		}
	}
}

#endif
