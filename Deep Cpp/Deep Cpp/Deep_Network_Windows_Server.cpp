#include "Deep_Network.h"

#if defined(DEEP_PLATFORM_WINDOWS)

namespace Deep
{
	namespace Network
	{
		void Server::Start(unsigned short port)
		{
			socket.Open();
			buffer = new char[maxBufferSize];
			socket.Bind(port);
		}

		void Server::Close()
		{
			socket.Close();
			delete[] buffer;
		}

		void Server::Tick()
		{
			while (socket.GetRemainingBytes() > 0)
			{
				int bytesReceived = socket.Receive(buffer, maxBufferSize, from);
				if (bytesReceived > 0)
				{
					unsigned int fromAddress = ntohl(from.sa_in.sin_addr.s_addr);
					unsigned int fromPort = ntohs(from.sa_in.sin_port);

					if (OnReceiveHandle) OnReceiveHandle(buffer, bytesReceived, fromAddress, fromPort);
				}
			}
		}
	}
}

#endif
