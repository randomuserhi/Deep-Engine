#pragma once

#include "Deep_Network.h"
#include <winsock2.h>

// Upon an error use WSAGetLastError() to get the error
// (should abstract it to make it work with linux, will need to use DEEP_SOCKET_ERROR_BLABLABLA
// - This will probably mean that my functions such as Bind() need to return actual errors instead of DEEP_SOCKET_ERROR or DEEP_NETWORK_ERROR

// TODO:: remove all std::cout calls and replace them with the proper DEEP_ERROR messages 
// => make the error messages similar to linux and windows WSA errors

// TODO:: Seperate this from another header such that winsock2.h is not imported onto the main file when this header is inlcuded

// TODO:: Implement Packet class
// TODO:: Implement Packet headers and acknowledgement bits etc (refer to NetworkTK C#)
// TODO:: Implement Packet splitting for large packets

namespace Deep
{
	bool IsBigEndian();
	int InitializeSockets();
	void ShutdownSockets();

	//https://stackoverflow.com/a/17925300/9642458 => Although not allowed by the standard, target compilers
	// support accessing inactive member of union.
	// Additionally, the use of a union also ensures alignment.
	union SocketAddr
	{
		sockaddr sa;
		sockaddr_in sa_in;
	};

	struct Address
	{
	private:
		SocketAddr sockAddr;

	public:
		unsigned char a;
		unsigned char b;
		unsigned char c;
		unsigned char d;
		unsigned short port;

		const SocketAddr& GetSocketAddr();

		Address(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port)
		{
			sockAddr = {};

			this->a = a;
			this->b = b;
			this->c = c;
			this->d = d;
			this->port = port;
		}

		friend bool operator==(const Address& sockA, const Address& sockB);
	};
	Deep_Inline const SocketAddr& Address::GetSocketAddr()
	{
		const unsigned int bitAddress = (a << 24) | (b << 16) | (c << 8) | d;

		sockAddr.sa_in.sin_family = AF_INET;
		sockAddr.sa_in.sin_addr.s_addr = htonl(bitAddress);
		sockAddr.sa_in.sin_port = htons(port);

		return sockAddr;
	}
	Deep_Inline bool operator==(const Address& lhs, const Address& rhs)
	{
		return (lhs.sockAddr.sa_in.sin_addr.s_addr == rhs.sockAddr.sa_in.sin_addr.s_addr) && (lhs.sockAddr.sa_in.sin_port == rhs.sockAddr.sa_in.sin_port);
	}

	namespace Impl
	{
		typedef int socklen_t;

		struct Socket
		{
		private:
			SOCKET socketFD;

			int GetRemainingBytes(u_long& bytes);

		public:
			Deep_Inline int GetSockName(SocketAddr& address)
			{
				socklen_t assignedAddressLen = sizeof address;
				if (getsockname(socketFD, &address.sa, &assignedAddressLen) != NO_ERROR)
					return DEEP_SOCKET_ERROR;
				return DEEP_SOCKET_NOERROR;
			}

			int Open();
			int Close();
			int Bind(unsigned short port);
			int Connect(const SocketAddr& address);
			int Connect(Address& address);
			int Send(const char* data, int dataSize, const SocketAddr& address);
			Deep_Inline int Send(const char* data, int dataSize, Address& address);
			int SendTo(const char* data, int dataSize, const SocketAddr& address);
			Deep_Inline int SendTo(const char* data, int dataSize, Address& address);
			int Receive(char* buffer, int maxBufferSize, int& bytesReceived, SocketAddr& fromAddress);
		};
		Deep_Inline int Socket::GetRemainingBytes(u_long& bytes)
		{
			const int result = ioctlsocket(socketFD, FIONREAD, &bytes);
			if (result != NOERROR)
			{
				std::cout << "Failed to get remaining bytes with error = " << WSAGetLastError() << '\n';
				return DEEP_SOCKET_ERROR;
			}
			return DEEP_SOCKET_NOERROR;
		}
		Deep_Inline int Socket::Connect(Address& address)
		{
			return Connect(address.GetSocketAddr());
		}
		Deep_Inline int Socket::Send(const char* data, int dataSize, Address& address)
		{
			return Send(data, dataSize, address.GetSocketAddr());
		}
		Deep_Inline int Socket::SendTo(const char* data, int dataSize, Address& address)
		{
			return SendTo(data, dataSize, address.GetSocketAddr());
		}
	}

	/*struct Server
	{
	private:
		SocketAddr from;

		char* buffer;
		unsigned int maxBufferSize;

	public:
		Socket socket;

		Server(unsigned int maxBufferSize = 256U)
		{
			socket = {};
			from = {};

			buffer = nullptr;
			this->maxBufferSize = maxBufferSize;

			OnReceiveHandle = nullptr;
		}

		void (*OnReceiveHandle)(const char*, int, unsigned int, unsigned int);

		void Start(unsigned short port);
		void Close();
		void Tick();

		int Send(const char* data, int dataSize, const SocketAddr& address)
		{
			return socket.Send(data, dataSize, address);
		};
	};*/
}
