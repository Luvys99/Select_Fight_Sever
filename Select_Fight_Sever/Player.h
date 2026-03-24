#pragma once
#include "NetStd.h"
#include "PacketDefine.h"
#include "Serialize_Buffer.h"

class Player
{
public:
	Player(SOCKET s, int id, int sid) : sock(s), id(id), sessionid(sid), dir(dfPACKET_MOVE_DIR_RR), x(200), y(300), realx(200), realy(300), isMoving(false), hp(100), xspeed(150.0f), yspeed(100.0f) {}
	~Player() { closesocket(sock); }

	SOCKET GetSocket() const { return sock; }
	int Getid() const { return id; }
	short GetX() const { return x; }
	short GetY() const { return y; }
	char Getdir() const { return dir; }
	char Gethp() const { return hp; }
	int Getsid() const { return sessionid; }

	// START, STOP할 때 클라에서 보낸 좌표 저장 ( 동기화 )
	void Startmove(char clientdir, short clientx, short clienty);
	void Stopmove(char clientdir, short clientx, short clienty);

	// ATTACK할 때 클라에서 보낸 좌표를 저장 ( 동기화 )
	void SyncPosition(char dir, short newx, short newy);

	// deltatime 만큼 서버에서 이동 처리하는 함수
	void UpdatePosition(float deltatime);

	// 플레이어 데미지 처리 Set함수
	char TakeDamage(int attackid, short damage);

	// 플레이어 공격 쿨타임 측정
	ULONGLONG GetLastAttackTime() { return lastAttackTime; }
	void SetLastAttackTime(ULONGLONG time) { lastAttackTime = time; }

	// 송신 함수
	void SendPacket(CMessage& msg)
	{
		SendQ.Enqueue((char*)msg.GetReadPtr(), msg.GetUseDataSize());
	}

public:
	RingBuffer RecvQ;
	RingBuffer SendQ;

private:
	SOCKET sock;
	int id;
	int sessionid;
	char dir;
	short x, y; // 통신용 정수 좌표
	float realx, realy; // 계산용 실수 좌표
	bool isMoving; // 현재 이동 중인가?
	char hp;
	float xspeed; // 프레임당 3
	float yspeed; // 프레임당 2

	// 마지막으로 공격한 시간 저장
	ULONGLONG lastAttackTime = 0;

};