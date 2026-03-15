#include "Server.h"
#include "NetworkSystem.h"
#include "PlayerList.h"
#include "SetSelect.h"

Server::Server()
{
    networksystem = new NetWorkSystem();
    playermgr = new PlayerManager();
    selector = new SetSelect();
}

Server::~Server()
{
    delete networksystem;
    delete playermgr;
    delete selector;
}

void Server::Run()
{
    // 윈속 ~ listen 설정
    if (!networksystem->InitNetwork(SERVER_PORT)) return;

    // Player socket을 저장할 백터
    vector<SOCKET> readsocks;
    vector<SOCKET> writesocks;

    // 프레임 로직을 50프레임에 맞춰서 처리하기 위한 시간 측정
    auto logicCheckTime = chrono::steady_clock::now();
    const auto frameDuration = chrono::milliseconds(20);

    // 1초당 이동속도 계산을 위한 변수
    const float fixedDeltatime = 0.02f;

    // 네트워크 로직 처리 + 프레임 로직 처리
    while (true)
    {
        // 매 루프 마다 벡터를 비워줘야 함
        readsocks.clear();
        writesocks.clear();

        // 소켓 셋에 등록할 소켓들 백터에 넣어서 인자로 전달 ( 종속적으로 변수가 함수의 인자로 전달되서 묶이지 않게 하기 위함 - 인자에 해당하는 구조체를 수정하면 전달받는 함수가 있는 cpp까지 리빌드 됨)
        for (int i = 0; i < playermgr->GetUseCount(); i++)
        {
            readsocks.push_back(playermgr->GetPlayer(i)->GetSocket());
            if (playermgr->GetPlayer(i)->SendQ.GetUseSize() > 0)
            {
                writesocks.push_back(playermgr->GetPlayer(i)->GetSocket());
            }
        }

        // select 함수 호출
        selector->SelectFunc(networksystem->GetListenSocket(), readsocks, writesocks);

        // 네트워크 이벤트 처리 함수
        CheckNetEvent();

        // 프레임 로직 ( 이동, 공격 )
        auto currenttime = chrono::steady_clock::now();
        if (currenttime >= logicCheckTime)
        {
            // 프레임 로직 처리
            UpdateFrame(fixedDeltatime);
            logicCheckTime += frameDuration;
        }
        
    }

    
    return;

}

// wset, rset 체크
void Server::CheckNetEvent()
{
    // Listen socket 체크
    if (FD_ISSET(networksystem->GetListenSocket(), selector->Getrset()))
    {
        HandleAccept();
    }
    
    // 개별 플레이어의 wset, rset 체크
    for (int i = 0; i < playermgr->GetUseCount(); i++)
    {
        SOCKET clientSock = playermgr->GetPlayer(i)->GetSocket();

        if (FD_ISSET(clientSock, selector->Getrset()))
        {
            // recv 함수 호출
            HandleRecv(i);
        }

        if (FD_ISSET(clientSock, selector->Getwset()))
        {
            //send함수 호출
            HandleSend(i);
        }
    }
   
    return;
}

void Server::HandleAccept()
{
    SOCKET client_sock;
    SOCKADDR_IN clientaddr;
    int addrlen = sizeof(clientaddr);

    client_sock = accept(networksystem->GetListenSocket(), (SOCKADDR*)&clientaddr, &addrlen);
    if (client_sock == INVALID_SOCKET)
    {
        wprintf(L"accept failed!! error_code : %d\n", WSAGetLastError());
        return;
    }

    // IP 문자열로 출력하기
    WCHAR ClientIP[16] = { 0 };
    InetNtop(AF_INET, &clientaddr.sin_addr, ClientIP, 16);
    wprintf(L"Connect IP : %s | ", ClientIP);
    
    // 플레이어가 접속하면 리스트에 등록하기
    int newplayeridx = playermgr->AddPlayer(client_sock);

    // 등록되었으면 메시지 패킷 보내기
    if (newplayeridx != -1)
    {
        int newplayerid = playermgr->GetPlayer(newplayeridx)->Getid();

        // 새로 접속한 클라이언트에게 캐릭터 정보 생성해서 보내기
        SendSC_CREATE_MY_CHARACTER(newplayeridx, newplayerid);
    
        // 새로 접속한 클라이언트에게 기존 플레이어들의 정보 보내기
        SendSC_CREATE_OTHER_CHARACTER(newplayeridx);

        // 새로 접속한 클라이언트의 정보를 기존 플레이어들에게 브로드 캐스트
        Broadcast_CREATE_OTHER_CHARACTER(newplayeridx);

    }

    return;
}

void Server::HandleRecv(int playeridx)
{
    Player* p = playermgr->GetPlayer(playeridx);
    if (p == nullptr) return;

    // 링버퍼에 다이렉트러 저장
    int Enqueuesize = p->RecvQ.DirectEnqueueSize();
    char* RecvQptr = p->RecvQ.GetRearBufferPtr();

    int recv_ret = recv(p->GetSocket(), RecvQptr, Enqueuesize, 0);
    if (recv_ret == SOCKET_ERROR)
    {
        if (WSAGetLastError() != WSAEWOULDBLOCK)
        {
            // 플레이어 접속 끊기
            DisconnectPlayer(playeridx);
            return;
        }
    }
    else if (recv_ret == 0)
    {
        // 정상 종료
        DisconnectPlayer(playeridx);
        return;
    }

    // Recv 받은 만큼 Rear 움직이기
    p->RecvQ.MoveRear(recv_ret);

    // 받은 데이터 링버퍼에서 꺼내서 패킷 단위로 잘라서 처리하는 함수
    ProcessPacketProtocol(playeridx);

    return;
}
 
void Server::HandleSend(int playeridx)
{

    Player* p = playermgr->GetPlayer(playeridx);
    if (p == nullptr) return;

    //보낼 것이 없으면 리턴
    if (p->SendQ.GetUseSize() <= 0) return;

    char buf[4096];
    int peeksize = p->SendQ.DirectDequeueSize();
    int peek_ret = p->SendQ.Peek(buf, peeksize);

    int send_ret = send(p->GetSocket(), buf, peek_ret, 0);
    if (send_ret == SOCKET_ERROR)
    {
        if (WSAGetLastError() != WSAEWOULDBLOCK)
        {
            //플레이어 접속 끊기
            DisconnectPlayer(playeridx);
            return;
        }
    }

    //Dequeue로 뺴지 않고 포인터만 옮기기
    if (send_ret > 0)
    {
        p->SendQ.MoveFront(send_ret);
    }

    return;
}

void Server::ProcessPacketProtocol(int playeridx)
{

    Player* p = playermgr->GetPlayer(playeridx);

    while (true)
    {
        // 헤더보다 꺼낼 데이터 크기가 작으면 처리 안함
        if (p->RecvQ.GetUseSize() < sizeof(PACKET_HEADER)) break;

        // 헤더를 먼저 꺼내서 확인
        PACKET_HEADER header;
        p->RecvQ.Peek((char*)&header, sizeof(header));

        // 헤더 코드 일치하는지 확인
        if (header.h_code != 0x89)
        {
            wprintf(L"Not Equal h_code \n");

            DisconnectPlayer(playeridx);
            return;
        }

        // 헤더밖에 안 오고 body가 오지 않았으면 처리 안함
        if (p->RecvQ.GetUseSize() < sizeof(header) + header.h_size) break;

        // 헤더는 검증되었으므로 빼내기
        p->RecvQ.MoveFront(sizeof(header));

        switch (header.h_type)
        {
            case dfPACKET_CS_MOVE_START:
            {
                CS_MOVE_START body;
                p->RecvQ.Dequeue((char*)&body, header.h_size);

                // SC_MOVE_START 메시지 보내는 함수
                ProcessMoveStart(playeridx, body.dir, body.x, body.y);
            }
            break;
            case dfPACKET_CS_MOVE_STOP:
            {
                CS_MOVE_STOP body;
                p->RecvQ.Dequeue((char*)&body, header.h_size);

                // SC_MOVE_STOP 메시지 보내는 함수
                ProcessMoveStop(playeridx, body.dir, body.x, body.y);
            }
            break;
            case dfPACKET_CS_ATTACK1:
            {
                // 먼저 디큐 하고 검증해야지 헤더는 이미 빠졌기 때문에 
                // 그냥 break 던지면 헤더 검증에서 에러 발생
                CS_ATTACK1 body;
                p->RecvQ.Dequeue((char*)&body, header.h_size);

                // 쿨타임이 지났는지 먼저 체크
                ULONGLONG currenttime = GetTickCount64();
                if (currenttime - p->GetLastAttackTime() < 300)
                {
                    break; // 300ms가 지나지 않았으면 공격 X
                }

                // 통과되었으면 마지막 공격 시간을 갱신
                p->SetLastAttackTime(currenttime);

                wprintf(L"PACKET_ATTACK1 # SessionID: %d / Dir: %d / X: %d / Y: %d\n",
                    p->Getsid(), body.dir, body.x, body.y);

                // SC_ATTACK1 메시지 보내는 함수
                ProcessAttack1(playeridx, body.dir, body.x, body.y);

                // 공격 판정 처리
                ProcessAttack(playeridx, 1);
            }
            break;
            case dfPACKET_CS_ATTACK2:
            {
                CS_ATTACK2 body;
                p->RecvQ.Dequeue((char*)&body, header.h_size);

                // 쿨타임이 지났는지 먼저 체크
                ULONGLONG currenttime = GetTickCount64();
                if (currenttime - p->GetLastAttackTime() < 300)
                {
                    break; // 300ms가 지나지 않았으면 공격 X
                }

                // 통과되었으면 마지막 공격 시간을 갱신
                p->SetLastAttackTime(currenttime);

                wprintf(L"PACKET_ATTACK2 # SessionID: %d / Dir: %d / X: %d / Y: %d\n",
                    p->Getsid(), body.dir, body.x, body.y);

                // SC_ATTACK2 메시지 보내는 함수
                ProcessAttack2(playeridx, body.dir, body.x, body.y);

                // 공격 판정 처리
                ProcessAttack(playeridx, 2);

            }
            break;
            case dfPACKET_CS_ATTACK3:
            {
                CS_ATTACK3 body;
                p->RecvQ.Dequeue((char*)&body, header.h_size);

                // 쿨타임이 지났는지 먼저 체크
                ULONGLONG currenttime = GetTickCount64();
                if (currenttime - p->GetLastAttackTime() < 300)
                {
                    break; // 300ms가 지나지 않았으면 공격 X
                }

                // 통과되었으면 마지막 공격 시간을 갱신
                p->SetLastAttackTime(currenttime); 

                wprintf(L"PACKET_ATTACK3 # SessionID: %d / Dir: %d / X: %d / Y: %d\n",
                    p->Getsid(), body.dir, body.x, body.y);

                // SC_ATTACK3 메시지 보내는 함수
                ProcessAttack3(playeridx, body.dir, body.x, body.y);

                // 공격 판정 처리
                ProcessAttack(playeridx, 3);


            }
            break;
            default:
                wprintf(L"No_Protocol Packet closeclient\n");
                DisconnectPlayer(playeridx);
                break;
        }
    }

}