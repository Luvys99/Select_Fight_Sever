#pragma once
#include "NetStd.h"

class NetWorkSystem
{
public:
	NetWorkSystem() {}
	~NetWorkSystem() { ClosetSocket(); }

	friend class SetSelect;
	bool InitNetwork(int port);
	void ClosetSocket();

	SOCKET GetListenSocket() const { return listen_socket;  }
private:
	SOCKET listen_socket = INVALID_SOCKET;
};