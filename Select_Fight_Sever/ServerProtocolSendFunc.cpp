#include "Server.h"
#include "PlayerList.h"
#include "Serialize_Buffer.h"
#include "Proxy.h"

// 새로 접속한 클라이언트에게 캐릭터 정보 생성해서 보내기
void Server::SendSC_CREATE_MY_CHARACTER(int playerlistidx, int playerid)
{

    Player* newplayer = playermgr->GetPlayer(playerlistidx);
    if (newplayer == nullptr) return;

    CMessage msg;
    proxy_create_my_character(msg, newplayer->Getid(), newplayer->Getdir(), newplayer->GetX(), newplayer->GetY(), newplayer->Gethp());

    // 직렬화된 데이터를 버퍼에서 송신큐로 저장
    newplayer->SendPacket(msg);

    return;
}

// 새로 접속한 클라이언트에게 기존 플레이어들의 정보 보내기
void Server::SendSC_CREATE_OTHER_CHARACTER(int playerlistidx)
{
  
    Player* newplayer = playermgr->GetPlayer(playerlistidx);
    if (newplayer == nullptr) return;

    int newplayerid = newplayer->Getid();

    for (int i = 0; i < playermgr->GetUserCount(); i++)
    {
        Player* oldplayer = playermgr->GetPlayer(i);

        // 리스트 순회 중에 내 ID를 찾게되면 패쓰
        if (oldplayer->Getid() == newplayerid) continue;

        // 직렬화 버퍼에 먼저 데이터 저장
        CMessage msg;

        proxy_create_my_other_character(msg, oldplayer->Getid(), oldplayer->Getdir(), oldplayer->GetX(), oldplayer->GetY(), oldplayer->Gethp());

        // 직렬화된 데이터를 버퍼에서 송신큐로 저장
        newplayer->SendPacket(msg);
        
    }

    return;
}

// 새로 접속한 클라이언트의 정보를 기존 플레이어들에게 브로드 캐스트
void Server::Broadcast_CREATE_OTHER_CHARACTER(int playerlistidx)
{
    Player* newplayer = playermgr->GetPlayer(playerlistidx);
    if (newplayer == nullptr) return;

    // 직렬화 버퍼에 먼저 데이터 저장
    CMessage msg;

    make_create_my_other_character(msg, newplayer->Getid(), newplayer->Getdir(), newplayer->GetX(), newplayer->GetY(), newplayer->Gethp());

    // 현재 존재하는 유저수만큼 리스트 순회
    for (int i = 0; i < playermgr->GetUserCount(); i++)
    {
        Player* oldplayer = playermgr->GetPlayer(i);

        // 내 아이디는 패스
        if (oldplayer->Getid() == newplayer->Getid()) continue;

        // 기존 유저의 송신큐에 새로 접속한 클라이언트의 정보를 저장 ( 보내기 )
        oldplayer->SendPacket(msg);

    }

    return;
}
