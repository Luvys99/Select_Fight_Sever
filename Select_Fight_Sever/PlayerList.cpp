#include "Server.h"
#include "PlayerList.h"
#include "NetworkSystem.h"
#include "SetSelect.h"


// Playerlist에서는 리스트에 추가만 하도록
int PlayerManager::AddPlayer(SOCKET s)
{

	// 클라이언트가 64개 이상이면 등록 불가
	if (usercount >= FD_SETSIZE) return -1;

	// 새 플레이어 객체 생성 및 ID 할당
	int newid = userid++;
	static int sessionid = 1000;
	int newsessionid = sessionid++;
	Player* newplayer = new Player(s, newid, newsessionid);

	int addedidx = usercount;
	playerlist[usercount++] = newplayer;

	wprintf(L"Sessionid : %d\n", newsessionid);

	return addedidx;
}

// playlist에서 해당 유저 제거하고 비어있는 리스트에 마지막 유저 정보 저장
void PlayerManager::ReMovePlayer(int idx)
{

	if (idx < 0 || idx >= usercount) return;

	wprintf(L"exit Sessionid : %d\n", playerlist[idx]->Getsid());
	// 해당 플레이어의 소켓을 닫고 리스트
	// 소켓은 소멸자에 의해 닫힘
	delete playerlist[idx];

	playerlist[idx] = playerlist[usercount - 1];
	playerlist[usercount - 1] = nullptr;
	usercount--;

}