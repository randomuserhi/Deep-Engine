#include "Deep.h"
#include "Deep_Network.h"

#include <signal.h>

static volatile bool running = true;

void closeSignalHandler(int dummy) {
	running = false;
}

void OnReceive(const char* Buffer, int BytesReceived, unsigned int FromAddress, unsigned int FromPort)
{
	std::cout << "Message received: " << *reinterpret_cast<const int*>(Buffer) << '\n';
}

int main()
{
	signal(SIGINT, closeSignalHandler);

	std::cout << "IsBigEndian: " << Deep::Network::IsBigEndian() << '\n';

	Deep::Network::InitializeSockets();

	Deep::Network::Server server{};
	server.OnReceiveHandle = &OnReceive;

	server.Start(DEEP_NETWORK_DEFAULTPORT);

	std::cout << "Server started on port: " << server.socket.GetPort() << '\n';

	int data = 10;

	Deep::Network::Address addr{ 127, 0, 0, 1, 56732 };
	while (running)
	{
		server.Tick();
		server.Send((char*)&data, sizeof data, addr.GetSocketAddr());
	}

	server.Close();
	Deep::Network::ShutdownSockets();
}
