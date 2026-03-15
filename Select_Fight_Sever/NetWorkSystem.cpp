#include "NetWorkSystem.h"

bool NetWorkSystem::InitNetwork(int port)
{
	// Winsock
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		wprintf(L"Winsock Init failed!! error_code : %d\n", WSAGetLastError());
		return false;
	}
	wprintf(L"WSAStartup Ok!!\n");

	//listen_socket
	listen_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_socket == INVALID_SOCKET)
	{
		wprintf(L"listen_sock create failed!! error_code : %d\n", WSAGetLastError());
		return false;
	}

	//Non-blocking 
	u_long on = 1;
	int non_blocking_ret = ioctlsocket(listen_socket, FIONBIO, &on);
	if (non_blocking_ret == SOCKET_ERROR)
	{
		wprintf(L"non_blocking socket failed error_code : %d\n", WSAGetLastError());
		return false;
	}

	//bind
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int bind_ret = bind(listen_socket, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (bind_ret == SOCKET_ERROR)
	{
		wprintf(L"listen_sock binding failed!! error_code : %d\n", WSAGetLastError());
		return false;
	}
	wprintf(L"Bind Ok!! Port : %d \n", port);

	//listen
	int listen_ret = listen(listen_socket, SOMAXCONN);
	if (listen_ret == SOCKET_ERROR)
	{
		wprintf(L"listen_sock listening failed!! error_code : %d\n", WSAGetLastError());
		return false;
	}
	wprintf(L"listen Ok!!\n");

	return true;
}

void NetWorkSystem::ClosetSocket()
{
	if (listen_socket != INVALID_SOCKET)
	{
		closesocket(listen_socket);
		listen_socket = INVALID_SOCKET;
	}
	WSACleanup();
}