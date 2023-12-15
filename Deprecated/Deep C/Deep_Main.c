#include "Deep.h"
#include "Deep_Containers.h"
#include "Deep_Math.h"
#include "Deep_Engine.h"
#include "Deep_Network.h"
#include "Deep_String.h"

#include <time.h>
#include <signal.h>

static volatile int running = DEEP_TRUE;

void closeSignalHandler(int dummy) {
	running = DEEP_FALSE;
}

void OnReceive(const char* Buffer, int BytesReceived, unsigned int FromAddress, unsigned int FromPort)
{
	printf("Message received: %i\n", *(const int*)Buffer);
}

int main()
{
	signal(SIGINT, closeSignalHandler);

#ifdef DEEP_DEBUG_MEMORY
	Deep_Debug_Memory_Start();
#endif

	struct Deep_ECS ECS;
	Deep_ECS_Create(&ECS);

	Deep_ECS_CreateComponent(&ECS, "Component Test", sizeof(int));

	const Deep_ECS_Handle type[] = { DEEP_ECS_ID, 10, 11, 12 };
	struct Deep_ECS_Archetype* archetype = Deep_ECS_GetArchetype(&ECS, type, 4);
	//TODO:: Pushes a new entity of given type, but doesn't append to hierarchy => make this not exclusive to be less confusing
	struct Deep_ECS_Reference reference = Deep_ECS_Archetype_Push(&ECS, archetype);
	//TODO:: Implement a better way of creating and naming an archetype
	((struct Deep_ECS_Id*)archetype->components.values->values)[reference.index].name = "Entity Test";

	//TODO:: Implement adding type to hierarchy better
	Deep_ECS_Handle handle = 11;
	size_t hash = Deep_UnorderedMap_Hash(&handle, sizeof handle, DEEP_UNORDEREDMAP_SEED);
	*Deep_UnorderedMap_Insert(Deep_ECS_Handle, Deep_ECS_Reference)(&ECS.hierarchy, hash, &handle)
		= reference;
	
	Deep_ECS_PrintHierarchy(&ECS);

	//TODO:: Handle removing entities better, as this just removes from the hierarchy, not from the archetype
	//    :: Maybe use tombstones or something to help with iteration, idk
	Deep_UnorderedMap_Erase(Deep_ECS_Handle, Deep_ECS_Reference)(&ECS.hierarchy, hash, &handle);

	printf("\n");

	Deep_ECS_PrintHierarchy(&ECS);

	*Deep_UnorderedMap_Insert(Deep_ECS_Handle, Deep_ECS_Reference)(&ECS.hierarchy, hash, &handle)
		= reference;

	printf("\n");

	Deep_ECS_PrintHierarchy(&ECS);

	Deep_ECS_Free(&ECS);

	printf("\n");

	struct Deep_Vector3 vec3 = Deep_Vector3_Create(10, 10, 0);
	Deep_Vector3_Scale_InPlace(&vec3, 2);
	printf("%f %f %f\n", vec3.x, vec3.y, vec3.z);

	printf("IsBigEndian: %i.\n", Deep_Network_IsBigEndian());

    Deep_Network_InitializeSockets();

	struct Deep_Network_Server server = Deep_Network_Server_Default;
	server.OnReceiveHandle = &OnReceive;

	Deep_Network_Socket_Open(&server.socket);
	Deep_Network_Server_Start(&server, DEEP_NETWORK_DEFAULTPORT);

	printf("Server started on port: %i.\n", server.socket.port);

	int data = 10;

	struct Deep_Network_Address addr = { .a = 127, .b = 0, .c = 0, .d = 1, .port = 56732 };
	while (running)
	{
		Deep_Network_Server_Tick(&server);
		Deep_Network_Address_Format(&addr);
		Deep_Network_Server_Send(&server, (char*)&data, sizeof data, &addr.sockAddr);
	}

	Deep_Network_Server_Close(&server);
    Deep_Network_ShutdownSockets();

#ifdef DEEP_DEBUG_MEMORY
	Deep_Debug_Memory_End();
#endif
}

