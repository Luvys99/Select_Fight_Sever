#pragma once
#include "NetStd.h"
#include "Player.h"

class PlayerManager
{
public:

	int AddPlayer(SOCKET s);

	void ReMovePlayer(int idx);

	int GetUserCount() const { return usercount; }
	Player* GetPlayer(int idx) { return playerlist[idx]; }

private:
	Player* playerlist[FD_SETSIZE] = { nullptr, };
	int usercount = 0;
	int userid = 0;
};

