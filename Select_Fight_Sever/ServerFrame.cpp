#include "Server.h"
#include "PlayerList.h"

void Server::UpdateFrame(float deltatime)
{

	for (int i = 0; i < playermgr->GetUserCount(); i++)
	{
		Player* p = playermgr->GetPlayer(i);
		if (p != nullptr)
		{
			// 각 플레이어 객체 스스로 위치를 갱신하게 만듬
			p->UpdatePosition(deltatime);
		}
	}

	// 플레이어가 이동하거나 유지되는 오브젝트들 충돌처리

}