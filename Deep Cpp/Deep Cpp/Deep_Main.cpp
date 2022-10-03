#include <signal.h>

#include "Deep.h"
#include "Deep_Network.h"

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

	//Deep::StartMemoryDebug();

	std::cout << "IsBigEndian: " << Deep::IsBigEndian() << '\n';

	Deep::InitializeSockets();
	Deep::Impl::Socket socket{};
	
	Deep::Address addr{ 127, 0, 0, 1, 56732 };
	
	socket.Open();
	socket.Bind(56732);

	while (running)
	{
		
	}

	socket.Close();

	//Deep::PrintAllocationMap();
	//Deep::EndMemoryDebug();
}
