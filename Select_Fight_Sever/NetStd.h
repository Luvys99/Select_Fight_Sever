#pragma once
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include "Ringbuffer.h"
#include <vector>
#include <chrono>
#pragma comment(lib, "ws2_32")

#define SERVER_PORT 5000
using namespace std;
