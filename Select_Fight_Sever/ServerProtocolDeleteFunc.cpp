#include "Server.h"
#include "PlayerList.h"
#include "Serialize_Buffer.h"
#include "Proxy.h"

// 종료된 유저의 정보를 다른 유저에게 전송
void Server::Broadcast_SC_DELETE_CHARACTER(int removeplayerid)
{
	CMessage msg;

	proxy_delete_character(msg, removeplayerid);

	for (int i = 0; i < playermgr->GetUserCount(); i++)
	{
		Player* otherplayer = playermgr->GetPlayer(i);

		if (otherplayer->Getid() == removeplayerid) continue;

		otherplayer->SendPacket(msg);

	}

}

void Server::DisconnectPlayer(int playeridx)
{
	Player* p = playermgr->GetPlayer(playeridx);
	if (p == nullptr) return;

	int removeid = p->Getid();

	// 먼저 종료된 유저가 있음을 브로드캐스트 후에
	Broadcast_SC_DELETE_CHARACTER(removeid);

	// 리스트에서 제거
	playermgr->ReMovePlayer(playeridx);

}