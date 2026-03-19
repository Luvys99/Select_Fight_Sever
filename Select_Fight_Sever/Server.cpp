#include "Server.h"
#include "NetworkSystem.h"
#include "PlayerList.h"
#include "SetSelect.h"
#include "Serialize_Buffer.h"

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
    // ------------------------------------------------------------------------------------------------------------------------------------------------------------
    // 네트워크 로직 처리 + 프레임 로직 업데이트
    // 
    // CheckNetEvent 함수는 클라이언트에서 보내는 메시지를 수신큐에 저장하고
    // 서버에서 로직 처리 이후에 송신큐에 저장해놓은 메시지를 보내기만 하는 네트워크 로직 처리 함수
    // -> 네트워크 로직 처리 함수에 프로토콜 메시지를 처리하는 함수를 추가하면 네트워크 로직만 계속 돌면서 빠르게 수신을 받고 송신해야되는데 게임 로직 처리하느라 부하가 생기고 스트레스 테스트하면 부하가 심해짐
    // 
    // ProcessPacketProtocol 함수는 네트워크 로직 처리와 별개로 메시지를 프로토콜 타입 별로 꺼내서 다른 클라이언트한테 알릴 것을 알리고 서버에서 충돌처리나 피격
    // 처리를 해야하면 피격처리 후에 SendQ에 저장만 하는 프레임 로직 업데이트 함수이다.
    // 
    // START, MOVE는 이동, 정지 패킷이 수신되었을 때 한 번 브로드 캐스트
    // ATTACK의 경우 두 가지 경우가 있는데 클라이언트가 공격을 요청했을 때 공격 모션을 먼저 서버에서 브로드 캐스트 해주고 이후에 피격판정하고 데미지 메시지를 브로드 캐스트해주는 방법과
    // 공격 모션을 먼저 보내지 않고 피격판정 이후에 브로드 캐스트 해주는 방법이 존재한다.
    // 
    // 그러나 키보드의 경우 조각감에 예민하고 마우스도 공격요청으로 딱 몬스터를 클릭했을 때 공격을 하진 않더라도 어떠한 반응이 일어나야지 조작감이 좋다고 느끼기 때문에 
    // 보통은 공격모션을 먼저 브로드캐스트하고 이후에 또 피격판정된 데미지 메시지를 브로드캐스트해서 Hp를 감소시키고 임팩트라든지 몬스터가 타격되었다라는 이팩트가 추가로 발생하게 만든다.
    // ------------------------------------------------------------------------------------------------------------------------------------------------------------
    while (true)
    {
        // 매 루프 마다 벡터를 비워줘야 함
        readsocks.clear();
        writesocks.clear();

        // 소켓 셋에 등록할 소켓들 백터에 넣어서 인자로 전달 
        // 종속적으로 변수가 함수의 인자로 전달되서 묶이지 않게 하기 위함 ( 인자에 해당하는 구조체를 수정하면 전달받는 함수가 있는 cpp까지 리빌드 됨 ) 
        for (int i = 0; i < playermgr->GetUserCount(); i++)
        {
            // rset셋은 항상 등록
            readsocks.push_back(playermgr->GetPlayer(i)->GetSocket());
            // wset은 송신큐에 보낼 것이 있으면 등록
            if (playermgr->GetPlayer(i)->SendQ.GetUseSize() > 0)
            {
                writesocks.push_back(playermgr->GetPlayer(i)->GetSocket());
            }
        }

        // select 함수 호출
        selector->SelectFunc(networksystem->GetListenSocket(), readsocks, writesocks);

        // 네트워크 처리 함수
        CheckNetEvent();

        // 프레임 로직 처리 ( 이동, 공격 )
        auto currenttime = chrono::steady_clock::now();
        while (currenttime >= logicCheckTime)
        {
            // 접속한 유저들의 수신버퍼에 들어있는 처리해야 할 프로토콜 메시지들을 순서대로 처리
            for (int i = 0; i < playermgr->GetUserCount(); i++)
            {
                
                // 인덱스를 넘겨주면 플레이어의 수신 큐에서 메시지를 꺼내서 처리
                ProcessPacketProtocol(i);
            }

            // 서버에서 프레임이 아닌 시간에 의존한 이동하도록 처리( 계산 )
            UpdateFrame(fixedDeltatime);
            // 프레임 50번 돌도록 기준되는 시간에 += 20ms씩 하면서 흐른 시간이랑 비교해서 1프레임당 20ms씩 지났다면 프레임 로직 돌리고 아직 20ms가 되지 않았으면 다음 프레임으로 넘김
            logicCheckTime += frameDuration;
        }
        
    }

    
    return;

}


void Server::CheckNetEvent()
{
    // Listen socket 체크
    if (FD_ISSET(networksystem->GetListenSocket(), selector->Getrset()))
    {
        HandleAccept();
    }
    
    // 개별 플레이어의 wset, rset 체크
    // 접속한 플레이어들의 소켓에서 발생한 이벤트 처리 
    for (int i = 0; i < playermgr->GetUserCount(); i++)
    {
        SOCKET clientSock = playermgr->GetPlayer(i)->GetSocket();

        if (FD_ISSET(clientSock, selector->Getrset()))
        {
            // recv 함수 호출해서 수신 큐에 저장
            HandleRecv(i);
        }

        if (FD_ISSET(clientSock, selector->Getwset()))
        {
            //send함수 호출해서 송신 큐에서 빼서 send
            HandleSend(i);
        }
    }
   
    return;
}

// 클라이언트가 최초로 접속할 때 서버에서 해당 프로토콜 메시지를 받으면 캐릭터 생성, 다른 클라이언트에게 새로운 캐릭터 정보 브로드 캐스트, 
// 새로운 클라이언트에게 기존 클라이언트 정보 송신, 송신큐에 넣었다가 다음 프레임에 송신하는 구조
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

    // 링버퍼에 다이렉트로 저장
    int Enqueuesize = p->RecvQ.DirectEnqueueSize();
    char* RecvQptr = p->RecvQ.GetRearBufferPtr();

    int recv_ret = recv(p->GetSocket(), RecvQptr, Enqueuesize, 0);
    if (recv_ret == SOCKET_ERROR)
    {
        // 비정상적인 종료라면
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
        // 비정상적인 종료
        if (WSAGetLastError() != WSAEWOULDBLOCK)
        {
            //플레이어 접속 끊기
            DisconnectPlayer(playeridx);
            return;
        }
    }

    // Dequeue로 뺴지 않고 포인터만 옮기기
    // send_ret가 Len이 0이면 0을 리턴할 수 있다. 
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
            // 코드가 일치하지 않으면 연결 끊기
            wprintf(L"Not Equal h_code \n");
            DisconnectPlayer(playeridx);
            return;
        }

        // 헤더밖에 안 오고 body가 오지 않았으면 처리 안함 ( 완성된 메시지가 없으면 처리 안함 )
        if (p->RecvQ.GetUseSize() < sizeof(header) + header.h_size) break;

        // 헤더는 검증되었으므로 제거
        p->RecvQ.MoveFront(sizeof(PACKET_HEADER));

        // 패킷 바디 크기를 빼서 직렬화 버퍼에 저장
        char temp[1400];
        p->RecvQ.Dequeue(temp, header.h_size);

        CMessage message;
        message.PutData(temp, header.h_size);

        // 프로토콜 타입 별로 메시지 처리
        switch (header.h_type)
        {
            case dfPACKET_CS_MOVE_START:
            {
                // SC_MOVE_START 메시지 보내는 함수
                ProcessMoveStart(playeridx, &message);
            }
            break;
            case dfPACKET_CS_MOVE_STOP:
            {
                // SC_MOVE_STOP 메시지 보내는 함수
                ProcessMoveStop(playeridx, &message);
            }
            break;
            case dfPACKET_CS_ATTACK1:
            {
                // -------------------------------------------------------------
                // 쿨타임이 지났는지 먼저 체크
                // 쿨타임은 현재 측정한 시간 - 마지막으로 공격했던 시간 
                // 공격패킷이 와서 처리하려는 그 시점의 흐른 시간과 이전에 마지막으로 공격했던 흐른 시간을 뺀 값이다.
                //
                ULONGLONG currenttime = GetTickCount64();
                if (currenttime - p->GetLastAttackTime() < 300)
                {
                    break; // 300ms가 지나지 않았으면 공격 X ( 이미 디큐했기 때문에 그대로 공격이 사라짐 씹힘 )
                }

                // 통과되었으면 마지막 공격 시간을 갱신
                p->SetLastAttackTime(currenttime);

                // 공격 모션 먼저 다른 유저에게 브로드 캐스트 ( 공격 메시지를 받고 그 공격 메시지를 다른 유저에게 브로드 캐스트 - 클라에서 그 패킷을 받으면 애니메이션이 작동될 것)
                ProcessAttack1(playeridx, &message);

                // 공격 판정 처리 후에 데미지 처리까지하고 브로드 캐스트 ( 송신 큐에 저장 )
                ProcessAttackDecision(playeridx, 1);
            }
            break;
            case dfPACKET_CS_ATTACK2:
            {
               
                // 쿨타임이 지났는지 먼저 체크
                ULONGLONG currenttime = GetTickCount64();
                if (currenttime - p->GetLastAttackTime() < 300)
                {
                    break; // 300ms가 지나지 않았으면 공격 X
                }

                // 통과되었으면 마지막 공격 시간을 갱신
                p->SetLastAttackTime(currenttime);

                // 공격 모션 먼저 다른 유저에게 브로드 캐스트
                ProcessAttack2(playeridx, &message);

                // 공격 판정 처리 후에 데미지 처리까지하고 브로드 캐스트 ( 송신 큐에 저장 )
                ProcessAttackDecision(playeridx, 2);

            }
            break;
            case dfPACKET_CS_ATTACK3:
            {

                // 쿨타임이 지났는지 먼저 체크
                ULONGLONG currenttime = GetTickCount64();
                if (currenttime - p->GetLastAttackTime() < 300)
                {
                    break; // 300ms가 지나지 않았으면 공격 X
                }

                // 통과되었으면 마지막 공격 시간을 갱신
                p->SetLastAttackTime(currenttime); 
                //p->SyncPosition(body.dir, body.x, body.y);

                // 공격 모션 먼저 다른 유저에게 브로드 캐스트,
                ProcessAttack3(playeridx, &message);

                // 공격 판정 처리 후에 데미지 처리까지하고 브로드 캐스트 ( 송신 큐에 저장 )
                ProcessAttackDecision(playeridx, 3);


            }
            break;
            default:
                wprintf(L"No_Protocol Packet closeclient\n");
                DisconnectPlayer(playeridx);
                break;
        }
    }

}