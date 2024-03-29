#pragma once

#include "Deep.h"

#define DEEP_NETWORK_DEFAULTPORT 57687

#define DEEP_SOCKET_NOERROR 0
#define DEEP_SOCKET_ERROR -1

#if defined(DEEP_PLATFORM_WINDOWS)
#include "Deep_Network_Windows.h"

#elif defined(DEEP_PLATFORM_MAC) || defined(DEEP_PLATFORM_UNIX)
#include "Deep_Network_Posix.h"

#endif
