#pragma once

// 전방 선언
class NetWorkSystem;
class PlayerManager;
class SetSelect;

// 최상위 클래스
class Server
{
public:
	Server();
	~Server();

	void Run();

private:
	// select 함수
	void CheckNetEvent();

	// listensock 체크 함수
	void HandleAccept();

	// Recv, Send 함수
	void HandleRecv(int playeridx);
	void HandleSend(int playeridx);

	// 캐릭터 생성, 기존 유저 정보 신규 유저에게 전송, 브로드캐스트 함수
	void SendSC_CREATE_MY_CHARACTER(int playerlistidx, int playerid);
	void SendSC_CREATE_OTHER_CHARACTER(int playerlistidx);
	void Broadcast_CREATE_OTHER_CHARACTER(int playerlistidx);

	// 캐릭터 삭제 
	void Broadcast_SC_DELETE_CHARACTER(int removeplayerid);
	void DisconnectPlayer(int playeridx);

	// Message Protocol 타입 별로 처리하는 함수
	void ProcessPacketProtocol(int playeridx);

	// MOVE START 처리 함수
	void ProcessMoveStart(int playeridx, char clientdir, short clientx, short clienty);
	void BroadCast_SC_MOVE_START(int playeridx, char clientdir, short clientx, short clienty);
	// MOVE STOP 처리 함수
	void ProcessMoveStop(int playeridx, char clientdir, short clientx, short clienty);
	void BroadCast_SC_MOVE_STOP(int playeridx, char clientdir, short clientx, short clienty);
	// ATTACK1 처리 함수
	void ProcessAttack1(int playeridx, char clientdir, short clientx, short clienty);
	void BroadCast_SC_Attack1(int playeridx, char clientdir, short clientx, short clienty);
	// ATTACK2 처리 함수
	void ProcessAttack2(int playeridx, char clientdir, short clientx, short clienty);
	void BroadCast_SC_Attack2(int playeridx, char clientdir, short clientx, short clienty);
	// ATTACK3 처리 함수
	void ProcessAttack3(int playeridx, char clientdir, short clientx, short clienty);
	void BroadCast_SC_Attack3(int playeridx, char clientdir, short clientx, short clienty);

	// 프레임 로직 업데이트 함수
	void UpdateFrame(float deltatime);

	// 충돌 처리 함수
	void ProcessAttack(int attackerIdx, char attackType);
	// 데미지 처리 브로드 캐스트
	void Broadcast_SC_DAMAGE(int attackidx, int targetid, char hp);

private:
	NetWorkSystem* networksystem;
	PlayerManager* playermgr;
	SetSelect* selector;
};