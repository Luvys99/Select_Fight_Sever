#include "Server.h"
#include "PlayerList.h"
#include <cmath> //abs 함수 ( 절대값 반환하는 함수 )
#include "ScreenDefine.h"
using namespace std;

// MOVE START 처리 함수
void Server::ProcessMoveStart(int playeridx, char clientdir, short clientx, short clienty)
{
	Player* p = playermgr->GetPlayer(playeridx);
	if (p == nullptr) return;

	short serverx = p->GetX();
	short servery = p->GetY();

	// x,y 좌표가 오차범위를 넘지 않으면 클라이언트 좌표를 신뢰한다.
	if (abs(serverx - clientx) <= dfERROR_RANGE && abs(servery - clienty) <= dfERROR_RANGE)
	{
        wprintf(L"PACKET_SC_MOVE_START | Session : %d, Direction : %d, X : %d, Y : %d\n", p->Getsid(), p->Getdir(), p->GetX(), p->GetY());
		p->move(clientdir, clientx, clienty);
	}
	else
	{
        // 좌표가 많이 틀어졌다면 강제 보정 ( 서버의 부하로 인한 서버 좌표로 강제 보정 )
		wprintf(L"wrong x, y ID : %d\n", p->Getid());
	}

	// 좌표가 처리되었으면 해당 유저의 좌표를 모든 유저에게 전송
	BroadCast_SC_MOVE_START(p->Getid(), p->Getdir(), p->GetX(), p->GetY());

    return;
}

//  MOVE START 브로드 캐스트 함수
void Server::BroadCast_SC_MOVE_START(int playerid, char clientdir, short clientx, short clienty)
{

    PACKET_HEADER header;
    header.h_code = 0x89;
    header.h_size = sizeof(SC_MOVE_START);
    header.h_type = dfPACKET_SC_MOVE_START;

    SC_MOVE_START body;
    body.id = playerid;
    body.dir = clientdir;
    body.x = clientx;
    body.y = clienty;

    // 모든 유저를 순회하면서
    for (int i = 0; i < playermgr->GetUseCount(); i++)
    {
        Player* otherPlayer = playermgr->GetPlayer(i);

        // 이동하는 당사자(나)에게는 이 패킷을 보낼 필요가 없습니다. (내가 움직인 거니까)
        if (otherPlayer->Getid() == playerid) continue;

        // 다른 유저들의 송신 큐에 패킷을 꽂아줍니다.
        otherPlayer->SendQ.Enqueue((char*)&header, sizeof(header));
        otherPlayer->SendQ.Enqueue((char*)&body, header.h_size);
    }

    return;
}


// MOVE STOP 처리 함수
void Server::ProcessMoveStop(int playeridx, char clientdir, short clientx, short clienty)
{

    Player* p = playermgr->GetPlayer(playeridx);
    if (p == nullptr) return;

    short serverx = p->GetX();
    short servery = p->GetY();

    if (abs(serverx - clientx) <= dfERROR_RANGE && abs(servery - clienty))
    {
        wprintf(L"PACKET_SC_MOVE_STOP | Session : %d, Direction : %d, X : %d, Y : %d\n", p->Getsid(), p->Getdir(), p->GetX(), p->GetY());
        p->move(clientdir, clientx, clienty);
    }
    else
    {
        // 좌표가 많이 틀어졌다면 강제 보정 ( 서버의 부하로 인한 서버 좌표로 강제 보정 )
        wprintf(L"wrong x, y ID : %d\n", p->Getid());
    }

    // 좌표가 처리되었으면 해당 유저의 좌표를 모든 유저에게 전송
    BroadCast_SC_MOVE_STOP(p->Getid(), p->Getdir(), p->GetX(), p->GetY());

    return;
}

// MOVE STOP 브로드캐스트 함수
void Server::BroadCast_SC_MOVE_STOP(int playerid, char clientdir, short clientx, short clienty)
{

    PACKET_HEADER header;
    header.h_code = 0x89;
    header.h_size = sizeof(SC_MOVE_STOP);
    header.h_type = dfPACKET_SC_MOVE_STOP;

    SC_MOVE_STOP body;
    body.id = playerid;
    body.dir = clientdir;
    body.x = clientx;
    body.y = clienty;

    for (int i = 0; i < playermgr->GetUseCount(); i++)
    {
        Player* otherPlayer = playermgr->GetPlayer(i);

        if (otherPlayer->Getid() == playerid) continue;

        otherPlayer->SendQ.Enqueue((char*)&header, sizeof(header));
        otherPlayer->SendQ.Enqueue((char*)&body, header.h_size);

    }

    return;
}

// ATTACK1 처리 함수
void Server::ProcessAttack1(int playeridx, char dir, short x, short y)
{
    Player* p = playermgr->GetPlayer(playeridx);
    if (p == nullptr) return;

    // 프레임 로직 : 충돌 판정, 데미지 계산

    // 프레임 로직 처리 이후 브로드 캐스트
    BroadCast_SC_Attack1(p->Getid(), dir, x, y);
        
    return;
}

// ATTACK1 브로드캐스트 함수
void Server::BroadCast_SC_Attack1(int playerid, char clientdir, short clientx, short clienty)
{
    PACKET_HEADER header;
    header.h_code = 0x89;
    header.h_size = sizeof(CS_ATTACK1);
    header.h_type = dfPACKET_CS_ATTACK1;

    SC_MOVE_STOP body;
    body.id = playerid;
    body.dir = clientdir;
    body.x = clientx;
    body.y = clienty;

    for (int i = 0; i < playermgr->GetUseCount(); i++)
    {
        Player* otherPlayer = playermgr->GetPlayer(i);

        if (otherPlayer->Getid() == playerid) continue;

        otherPlayer->SendQ.Enqueue((char*)&header, sizeof(header));
        otherPlayer->SendQ.Enqueue((char*)&body, header.h_size);

    }

    return;
}

// ATTACK2 처리 함수
void Server::ProcessAttack2(int playeridx, char dir, short x, short y)
{
    Player* p = playermgr->GetPlayer(playeridx);
    if (p == nullptr) return;

    // 프레임 로직 : 충돌 판정, 데미지 계산

    // 프레임 로직 처리 이후 브로드 캐스트
    BroadCast_SC_Attack2(p->Getid(), dir, x, y);

    return;
}

// ATTACK2 브로드캐스트 함수
void Server::BroadCast_SC_Attack2(int playerid, char clientdir, short clientx, short clienty)
{
    PACKET_HEADER header;
    header.h_code = 0x89;
    header.h_size = sizeof(CS_ATTACK2);
    header.h_type = dfPACKET_CS_ATTACK2;

    SC_MOVE_STOP body;
    body.id = playerid;
    body.dir = clientdir;
    body.x = clientx;
    body.y = clienty;

    for (int i = 0; i < playermgr->GetUseCount(); i++)
    {
        Player* otherPlayer = playermgr->GetPlayer(i);

        if (otherPlayer->Getid() == playerid) continue;

        otherPlayer->SendQ.Enqueue((char*)&header, sizeof(header));
        otherPlayer->SendQ.Enqueue((char*)&body, header.h_size);

    }

    return;
}

// ATTACK3 처리 함수
void Server::ProcessAttack3(int playeridx, char dir, short x, short y)
{
    Player* p = playermgr->GetPlayer(playeridx);
    if (p == nullptr) return;

    // 프레임 로직 : 충돌 판정, 데미지 계산

    // 프레임 로직 처리 이후 브로드 캐스트
    BroadCast_SC_Attack3(p->Getid(), dir, x, y);

    return;
}

// ATTACK3 브로드캐스트 함수
void Server::BroadCast_SC_Attack3(int playerid, char clientdir, short clientx, short clienty)
{
    PACKET_HEADER header;
    header.h_code = 0x89;
    header.h_size = sizeof(CS_ATTACK3);
    header.h_type = dfPACKET_CS_ATTACK3;

    SC_MOVE_STOP body;
    body.id = playerid;
    body.dir = clientdir;
    body.x = clientx;
    body.y = clienty;

    for (int i = 0; i < playermgr->GetUseCount(); i++)
    {
        Player* otherPlayer = playermgr->GetPlayer(i);

        if (otherPlayer->Getid() == playerid) continue;

        otherPlayer->SendQ.Enqueue((char*)&header, sizeof(header));
        otherPlayer->SendQ.Enqueue((char*)&body, header.h_size);

    }

    return;
}