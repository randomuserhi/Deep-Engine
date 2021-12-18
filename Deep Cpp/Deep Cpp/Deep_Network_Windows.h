#pragma once

#include <winsock2.h>

namespace Deep 
{
	bool IsBigEndian();
	int InitializeSockets();
	void ShutdownSockets();

	typedef int socklen_t;

	//https://stackoverflow.com/a/17925300/9642458 => use of a union also ensures alignment
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
	Deep_Inline bool operator==(const Address& lhs, const Address& rhs)
	{
		return (lhs.sockAddr.sa_in.sin_addr.s_addr == rhs.sockAddr.sa_in.sin_addr.s_addr) && (lhs.sockAddr.sa_in.sin_port == rhs.sockAddr.sa_in.sin_port);
	}

	struct Socket
	{
	private:
		SOCKET socketFD;
		unsigned short port;

	public:
		unsigned short GetPort();

		int Open();
		int Close();
		int Bind(unsigned short port);
		int GetRemainingBytes();
		int Send(const char* data, int dataSize, const SocketAddr& address);
		int Send(const char* data, int dataSize, Address& address)
		{
			return Send(data, dataSize, address.GetSocketAddr());
		}
		int Receive(char* buffer, int maxBufferSize, SocketAddr& fromAddress);
	};

	class Server
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
	};
}

