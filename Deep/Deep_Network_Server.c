#include <stdio.h>
#include <stdint.h>

#include "Deep_Network.h"

#if defined(DEEP_PLATFORM_WINDOWS)

const struct Deep_Network_Server Deep_Network_Server_Default = {.maxBufferSize = 256, .OnReceiveHandle = NULL};

void Deep_Network_Server_Start(struct Deep_Network_Server* deepServer, unsigned short port)
{
	deepServer->buffer = malloc(deepServer->maxBufferSize * sizeof(unsigned char*));
	Deep_Network_Socket_Bind(&deepServer->socket, port);
}

void Deep_Network_Server_Close(struct Deep_Network_Server* deepServer)
{
	Deep_Network_Socket_Close(&deepServer->socket);
	free(deepServer->buffer);
}

void Deep_Network_Server_Tick(struct Deep_Network_Server* deepServer)
{
	while (Deep_Network_Socket_GetRemainingBytes(&deepServer->socket) > 0)
	{
		int bytesReceived = Deep_Network_Socket_Receive(&deepServer->socket, deepServer->buffer, deepServer->maxBufferSize, &deepServer->from);
		if (bytesReceived > 0)
		{
			unsigned int fromAddress = ntohl(deepServer->from.sa_in.sin_addr.s_addr);
			unsigned int fromPort = ntohs(deepServer->from.sa_in.sin_port);

			if (deepServer->OnReceiveHandle) deepServer->OnReceiveHandle(deepServer->buffer, bytesReceived, fromAddress, fromPort);
		}
	}
}

#endif
