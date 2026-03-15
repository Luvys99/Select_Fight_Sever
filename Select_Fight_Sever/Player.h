#pragma once
#include "NetStd.h"
#include "PacketDefine.h"

class Player
{
public:
	Player(SOCKET s, int id, int sid) : sock(s), id(id), sessionid(sid), dir(dfPACKET_MOVE_DIR_RR), x(200), y(300), hp(100) {}
	~Player() { closesocket(sock); }

	SOCKET GetSocket() const { return sock; }
	int Getid() const { return id; }
	short GetX() const { return x; }
	short GetY() const { return y; }
	char Getdir() const { return dir; }
	char Gethp() const { return hp; }
	int Getsid() const { return sessionid; }

	void move(char clientdir, short clientx, short clienty)
	{
		dir = clientdir;
		x = clientx;
		y = clienty;
	}

public:
	RingBuffer RecvQ;
	RingBuffer SendQ;

private:
	SOCKET sock;
	int id;
	int sessionid;
	char dir;
	short x, y;
	char hp;
};