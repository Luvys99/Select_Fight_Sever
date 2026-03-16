#include "Server.h"
#include "PlayerList.h"

// 새로 접속한 클라이언트에게 캐릭터 정보 생성해서 보내기
void Server::SendSC_CREATE_MY_CHARACTER(int playerlistidx, int playerid)
{

    Player* newplayer = playermgr->GetPlayer(playerlistidx);
    if (newplayer == nullptr) return;


    PACKET_HEADER header;
    header.h_code = 0x89;
    header.h_size = sizeof(SC_CREATE_CHARACTER);
    header.h_type = dfPACKET_SC_CREATE_MY_CHARACTER;

    SC_CREATE_CHARACTER body;
    body.id = playerid;
    body.dir = newplayer->Getdir();
    body.x = newplayer->GetX();
    body.y = newplayer->GetY();
    body.hp = newplayer->Gethp();

    // 전송하기 위해 송신큐에 저장
    newplayer->SendQ.Enqueue((char*)&header, sizeof(header));
    newplayer->SendQ.Enqueue((char*)&body, sizeof(body));

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

        PACKET_HEADER header;
        header.h_code = 0x89;
        header.h_size = sizeof(SC_CREATE_OTHER_CHARACTER);
        header.h_type = dfPACKET_SC_CREATE_OTHER_CHARACTER;

        SC_CREATE_OTHER_CHARACTER body;
        body.id = oldplayer->Getid();
        body.dir = oldplayer->Getdir();
        body.x = oldplayer->GetX();
        body.y = oldplayer->GetY();
        body.hp = oldplayer->Gethp();

        newplayer->SendQ.Enqueue((char*)&header, sizeof(header));
        newplayer->SendQ.Enqueue((char*)&body, sizeof(body));
        
    }

    return;
}

// 새로 접속한 클라이언트의 정보를 기존 플레이어들에게 브로드 캐스트
void Server::Broadcast_CREATE_OTHER_CHARACTER(int playerlistidx)
{
    Player* newplayer = playermgr->GetPlayer(playerlistidx);
    if (newplayer == nullptr) return;

    PACKET_HEADER header;
    header.h_code = 0x89;
    header.h_size = sizeof(SC_CREATE_OTHER_CHARACTER);
    header.h_type = dfPACKET_SC_CREATE_OTHER_CHARACTER;

    SC_CREATE_OTHER_CHARACTER body;
    body.id = newplayer->Getid();
    body.dir = newplayer->Getdir();
    body.x = newplayer->GetX();
    body.y = newplayer->GetY();
    body.hp = newplayer->Gethp();

    // 현재 존재하는 유저수만큼 리스트 순회
    for (int i = 0; i < playermgr->GetUserCount(); i++)
    {
        Player* oldplayer = playermgr->GetPlayer(i);

        // 내 아이디는 패스
        if (oldplayer->Getid() == newplayer->Getid()) continue;

        // 기존 유저의 송신큐에 새로 접속한 클라이언트의 정보를 저장( 보내기 )
        oldplayer->SendQ.Enqueue((char*)&header, sizeof(header));
        oldplayer->SendQ.Enqueue((char*)&body, sizeof(body));

    }

    return;
}
