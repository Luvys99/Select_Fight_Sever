#include "Server.h"
#include "PlayerList.h"
#include <cmath> //abs 함수 ( 절대값 반환하는 함수 )
#include "ScreenDefine.h"
#include "Serialize_Buffer.h"
#include "Proxy.h"
using namespace std;

// MOVE START 처리 함수
void Server::ProcessMoveStart(int playeridx, char clientdir, short clientx, short clienty) 
{
    Player* p = playermgr->GetPlayer(playeridx);
	if (p == nullptr) return;

    // 현재 서버가 가지고 있는 플레이어의 좌표를 가져와서 클라이언트에서 준 좌표와 비교
	short serverx = p->GetX();
	short servery = p->GetY();

	// x,y 좌표가 오차범위를 넘지 않으면 클라이언트 좌표를 신뢰한다.
	if (abs(serverx - clientx) <= dfERROR_RANGE && abs(servery - clienty) <= dfERROR_RANGE)
	{
        // 서버에 저장된 플레이어의 데이터에다가 클라이언트 좌표를 저장
		p->Startmove(clientdir, clientx, clienty);
        wprintf(L"PACKET_SC_MOVE_START | Session : %d, Direction : %d, X : %d, Y : %d\n", p->Getsid(), p->Getdir(), p->GetX(), p->GetY());
	}
	else
	{
        // 좌표가 많이 틀어졌다면 강제 보정 ( 서버의 부하로 인한 서버 좌표로 강제 보정 )
		wprintf(L"MoveStart wrong x, y ID : %d\n", p->Getsid());
        
        // 강제 보정 ( 현재 서버의 좌표를 클라이언트에 전송 ) - 서버의 좌표를 신뢰함 
        // 서버가 프레임 로직을 처리하지 못해서 클라이언트만 이동하고 서버는 이동하지 못해 좌표가 틀어졌기 때문에 서버의 좌표로 다시 클라이언트한테 전송
        // Fix_userpos(serverx, servery);
	}

	// 좌표가 처리되었으면 해당 유저의 좌표를 모든 유저에게 전송
	BroadCast_SC_MOVE_START(p->Getid(), p->Getdir(), p->GetX(), p->GetY());

    return;
}

//  MOVE START 브로드 캐스트 함수
void Server::BroadCast_SC_MOVE_START(int playerid, char clientdir, short clientx, short clienty)
{
    CMessage msg;

    proxy_move_start(msg, playerid, clientdir, clientx, clienty);

    // 모든 유저를 순회하면서
    for (int i = 0; i < playermgr->GetUserCount(); i++)
    {
        Player* otherPlayer = playermgr->GetPlayer(i);

        // 본인 제외
        if (otherPlayer->Getid() == playerid) continue;

        // 다른 유저에게 브로드 캐스트
        
        otherPlayer->SendPacket(msg);
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

    if (abs(serverx - clientx) <= dfERROR_RANGE && abs(servery - clienty) <= dfERROR_RANGE)
    {
        p->Stopmove(clientdir, clientx, clienty);
        wprintf(L"PACKET_SC_MOVE_STOP | Session : %d, Direction : %d, X : %d, Y : %d\n", p->Getsid(), p->Getdir(), p->GetX(), p->GetY());
    }
    else
    {
        // 좌표가 많이 틀어졌다면 강제 보정 ( 서버의 부하로 인한 서버 좌표로 강제 보정 )
        wprintf(L"MoveStopwrong x, y ID : %d\n", p->Getsid());

        // 강제 보정 ( 현재 서버의 좌표를 클라이언트에 전송 )
        // Fix_userpos(serverx, servery);
    }

    // 좌표가 처리되었으면 해당 유저의 좌표를 모든 유저에게 전송
    BroadCast_SC_MOVE_STOP(p->Getid(), p->Getdir(), p->GetX(), p->GetY());

    return;
}

// MOVE STOP 브로드캐스트 함수
void Server::BroadCast_SC_MOVE_STOP(int playerid, char clientdir, short clientx, short clienty)
{
    CMessage msg;

    proxy_move_stop(msg, playerid, clientdir, clientx, clienty);

    for (int i = 0; i < playermgr->GetUserCount(); i++)
    {
        Player* otherPlayer = playermgr->GetPlayer(i);

        if (otherPlayer->Getid() == playerid) continue;

        otherPlayer->SendPacket(msg);

    }

    return;
}

// ATTACK1 처리 함수
void Server::ProcessAttack1(int playeridx, char clientdir, short clientx, short clienty)
{
    Player* p = playermgr->GetPlayer(playeridx);
    if (p == nullptr) return;

    // -------------------------------------------------------------
    // 쿨타임이 지났는지 먼저 체크
    // 쿨타임은 현재 측정한 시간 - 마지막으로 공격했던 시간 
    // 공격패킷이 와서 처리하려는 그 시점의 흐른 시간과 이전에 마지막으로 공격했던 흐른 시간을 뺀 값이다.
    //
    ULONGLONG currenttime = GetTickCount64();
    if (currenttime - p->GetLastAttackTime() < 300)
    {
        return; // 300ms가 지나지 않았으면 공격 X ( 이미 디큐했기 때문에 그대로 공격이 사라짐 씹힘 )
    }

    // 통과되었으면 마지막 공격 시간을 갱신
    p->SetLastAttackTime(currenttime);

    wprintf(L"PACKET_ATTACK1 # SessionID: %d / Dir: %d / X: %d / Y: %d\n",
        p->Getsid(), clientdir, clientx, clienty);

    // 공격 모션에 대한 브로드 캐스트 ( 클라이언트에서 공격 요청을 보냈으면 거기에 해당하는 액션이 다른 클라이언트에 나오도록 브로드 캐스트 )
    BroadCast_SC_Attack1(p->Getid(), clientdir, clientx, clienty);

    // 공격 판정 처리 후에 데미지 처리까지하고 브로드 캐스트 ( 송신 큐에 저장 )
    ProcessAttackDecision(p->Getid(), 1);
        
    return;
}

// ATTACK1 브로드캐스트 함수
void Server::BroadCast_SC_Attack1(int playerid, char clientdir, short clientx, short clienty)
{
    CMessage msg;

    proxy_attack1(msg, playerid, clientdir, clientx, clienty);

    for (int i = 0; i < playermgr->GetUserCount(); i++)
    {
        Player* otherPlayer = playermgr->GetPlayer(i);

        if (otherPlayer->Getid() == playerid) continue;

        otherPlayer->SendPacket(msg);

    }

    return;
}

// ATTACK2 처리 함수
void Server::ProcessAttack2(int playeridx, char clientdir, short clientx, short clienty)
{
    Player* p = playermgr->GetPlayer(playeridx);
    if (p == nullptr) return;

    // 쿨타임이 지났는지 먼저 체크
    ULONGLONG currenttime = GetTickCount64();
    if (currenttime - p->GetLastAttackTime() < 300)
    {
        return; // 300ms가 지나지 않았으면 공격 X
    }

    // 통과되었으면 마지막 공격 시간을 갱신
    p->SetLastAttackTime(currenttime);

    wprintf(L"PACKET_ATTACK2 # SessionID: %d / Dir: %d / X: %d / Y: %d\n",
        p->Getsid(), clientdir, clientx, clienty);

    // 공격 모션에 대한 브로드 캐스트 ( 클라이언트에서 공격 요청을 보냈으면 거기에 해당하는 액션이 다른 클라이언트에 나오도록 브로드 캐스트 )
    BroadCast_SC_Attack2(p->Getid(), clientdir, clientx, clienty);

    // 공격 판정 처리 후에 데미지 처리까지하고 브로드 캐스트 ( 송신 큐에 저장 )
    ProcessAttackDecision(p->Getid(), 2);

    return;
}

// ATTACK2 브로드캐스트 함수
void Server::BroadCast_SC_Attack2(int playerid, char clientdir, short clientx, short clienty)
{
    CMessage msg;

    proxy_attack2(msg, playerid, clientdir, clientx, clienty);

    for (int i = 0; i < playermgr->GetUserCount(); i++)
    {
        Player* otherPlayer = playermgr->GetPlayer(i);

        if (otherPlayer->Getid() == playerid) continue;

        otherPlayer->SendPacket(msg);

    }

    return;
}

// ATTACK3 처리 함수
void Server::ProcessAttack3(int playeridx, char clientdir, short clientx, short clienty)
{
    Player* p = playermgr->GetPlayer(playeridx);
    if (p == nullptr) return;

    // 쿨타임이 지났는지 먼저 체크
    ULONGLONG currenttime = GetTickCount64();
    if (currenttime - p->GetLastAttackTime() < 300)
    {
        return; // 300ms가 지나지 않았으면 공격 X
    }

    // 통과되었으면 마지막 공격 시간을 갱신
    p->SetLastAttackTime(currenttime);

    wprintf(L"PACKET_ATTACK3 # SessionID: %d / Dir: %d / X: %d / Y: %d\n",
        p->Getsid(), clientdir, clientx, clienty);

    // 공격 모션에 대한 브로드 캐스트 ( 클라이언트에서 공격 요청을 보냈으면 거기에 해당하는 액션이 다른 클라이언트에 나오도록 브로드 캐스트 )
    BroadCast_SC_Attack3(p->Getid(), clientdir, clientx, clienty);

    // 공격 판정 처리 후에 데미지 처리까지하고 브로드 캐스트 ( 송신 큐에 저장 )
    ProcessAttackDecision(p->Getid(), 3);

    return;
}

// ATTACK3 브로드캐스트 함수
void Server::BroadCast_SC_Attack3(int playerid, char clientdir, short clientx, short clienty)
{
    CMessage msg;

    proxy_attack3(msg, playerid, clientdir, clientx, clienty);

    for (int i = 0; i < playermgr->GetUserCount(); i++)
    {
        Player* otherPlayer = playermgr->GetPlayer(i);

        if (otherPlayer->Getid() == playerid) continue;

        otherPlayer->SendPacket(msg);

    }

    return;
}