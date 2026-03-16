#include "Server.h"
#include "PlayerList.h"
#include "ScreenDefine.h"

void Server::ProcessAttackDecision(int attackerIdx, char attackType)
{
    // 1. 공격자 정보 가져오기
    Player* attacker = playermgr->GetPlayer(attackerIdx);
    if (attacker == nullptr) return;

    // 2. 공격 타입에 따른 사거리(Range) 세팅
    int rangeX = 0;
    int rangeY = 0;
    char damageValue = 0;

    switch (attackType)
    {
    case 1: 
        rangeX = dfATTACK1_RANGE_X; 
        rangeY = dfATTACK1_RANGE_Y; 
        damageValue = 2;
        break; // 80, 10
    case 2: 
        rangeX = dfATTACK2_RANGE_X; 
        rangeY = dfATTACK2_RANGE_Y; 
        damageValue = 2;
        break; // 90, 10
    case 3: 
        rangeX = dfATTACK3_RANGE_X; 
        rangeY = dfATTACK3_RANGE_Y; 
        damageValue = 6;
        break; // 100, 20
    default: return; // 잘못된 공격 타입
    }

    // 3. 서버에 있는 모든 유저를 순회하며 사거리 안에 있는지(충돌) 검사!
    for (int i = 0; i < playermgr->GetUserCount(); i++)
    {
        Player* target = playermgr->GetPlayer(i);

        // 방어 코드: 없는 유저이거나, 자기 자신이거나
        if (target == nullptr || target->Getid() == attacker->Getid() )
            continue;

        // X, Y 좌표 차이 계산
        int diffX = target->GetX() - attacker->GetX();
        int diffY = abs(target->GetY() - attacker->GetY()); // Y는 절대값!

        bool isHit = false;

        // 4. 타격 판정 로직 (Y축이 범위 안쪽인지 먼저 검사하여 최적화)
        if (diffY <= rangeY)
        {
            // 내가 바라보는 방향에 따라 X축 검사를 다르게 함!
            switch (attacker->Getdir())
            {
            case dfPACKET_MOVE_DIR_LL: // 왼쪽을 보고 있다면?
                // 적이 나보다 왼쪽에 있고(diffX < 0), 그 거리가 rangeX 이내라면 히트!
                if (diffX <= 0 && abs(diffX) <= rangeX) isHit = true;
                break;

            case dfPACKET_MOVE_DIR_RR: // 오른쪽을 보고 있다면?
                // 적이 나보다 오른쪽에 있고(diffX > 0), 그 거리가 rangeX 이내라면 히트!
                if (diffX >= 0 && diffX <= rangeX) isHit = true;
                break;

                
            }
        }

        // 5. 맞았다면 데미지 처리!
        if (isHit)
        {
            // target->TakeDamage(데미지 계산);
            char remainHp = target->TakeDamage(attacker->Getid(), damageValue);

            // 맞은 유저의 체력이 깎였다는 패킷을 모두에게 방송!
            Broadcast_SC_DAMAGE(attacker->Getid(), target->Getid(), remainHp);

            if (remainHp <= 0)
            {
                DisconnectPlayer(i);
            }

        }

    }
}

void Server::Broadcast_SC_DAMAGE(int attackid, int targetid , char hp)
{
    PACKET_HEADER header;
    header.h_code = 0x89; 
    header.h_size = sizeof(SC_DAMAGE);
    header.h_type = dfPACKET_SC_DAMAGE;

    SC_DAMAGE body;
    body.attack_id = attackid;
    body.damage_id = targetid;
    body.hp = hp;

    // 모든 접속자에게 전송
    for (int i = 0; i < playermgr->GetUserCount(); i++)
    {
        Player* p = playermgr->GetPlayer(i);
        if (p == nullptr) continue;

        // Send 큐에 헤더와 바디 밀어넣기
        p->SendQ.Enqueue((char*)&header, sizeof(PACKET_HEADER));
        p->SendQ.Enqueue((char*)&body, sizeof(SC_DAMAGE));
    }
}
