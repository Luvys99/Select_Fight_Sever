#include "Player.h"
#include "ScreenDefine.h"

void Player::Startmove(char clientdir, short clientx, short clienty)
{
	isMoving = true;
	dir = clientdir;
	x = clientx;
	y = clienty;
	realx = (float)clientx; // 실수 좌표로도 저장 ( 계산을 위함 )
	realy = (float)clienty;

}

void Player::Stopmove(short clientx, short clienty)
{
	isMoving = false;
	x = clientx;
	y = clienty;

	realx = (float)clientx; // 실수 좌표로도 저장 ( 계산을 위함 )
	realy = (float)clienty;

}

void Player::SyncPosition(char dir, short newx, short newy)
{
	this->dir = dir;
	this->x = newx;
	this->y = newy;
}

void Player::UpdatePosition(float deltatime)
{
	if (!isMoving) return; // 움직이고 있지 않다면 계산 X

	switch (dir)
	{
	case dfPACKET_MOVE_DIR_LL: 
		realx -= xspeed * deltatime;
		break;
	case dfPACKET_MOVE_DIR_LU:
		realx -= xspeed * deltatime;
		realy -= yspeed * deltatime;
		break;
	case dfPACKET_MOVE_DIR_UU:
		realy -= yspeed * deltatime;
		break;
	case dfPACKET_MOVE_DIR_RU:
		realx += xspeed * deltatime;
		realy -= yspeed * deltatime;
		break;
	case dfPACKET_MOVE_DIR_RR:
		realx += xspeed * deltatime;
		break;
	case dfPACKET_MOVE_DIR_RD:
		realx += xspeed * deltatime;
		realy += yspeed * deltatime;
		break;
	case dfPACKET_MOVE_DIR_DD:
		realy += yspeed * deltatime;
		break;
	case dfPACKET_MOVE_DIR_LD:
		realx -= xspeed * deltatime;
		realy += yspeed * deltatime;
		break;
	}

	bool hitwall = false;

	if (realx >= dfRANGE_MOVE_RIGHT)
	{
		realx = dfRANGE_MOVE_RIGHT;
		hitwall = true;
	}
	else if (realx <= dfRANGE_MOVE_LEFT)
	{
		realx = dfRANGE_MOVE_LEFT+1;
		hitwall = true;
	}

	if (realy >= dfRANGE_MOVE_BOTTOM)
	{
		realy = dfRANGE_MOVE_BOTTOM;
		hitwall = true;
	}
	else if (realy <= dfRANGE_MOVE_TOP)
	{
		realy = dfRANGE_MOVE_TOP;
		hitwall = true;
	}

	// 벽에 닿으면 이동처리를 하지 않음
	if (hitwall)
	{
		isMoving = false;
		return;
	}

	x = (short)realx;
	y = (short)realy;

	const wchar_t* dirStr = L"??";
	switch (dir)
	{
	case dfPACKET_MOVE_DIR_LL: dirStr = L"LL"; break;
	case dfPACKET_MOVE_DIR_RR: dirStr = L"RR"; break;
	case dfPACKET_MOVE_DIR_UU: dirStr = L"UU"; break;
	case dfPACKET_MOVE_DIR_DD: dirStr = L"DD"; break;
	case dfPACKET_MOVE_DIR_LU: dirStr = L"LU"; break;
	case dfPACKET_MOVE_DIR_RU: dirStr = L"RU"; break;
	case dfPACKET_MOVE_DIR_LD: dirStr = L"LD"; break;
	case dfPACKET_MOVE_DIR_RD: dirStr = L"RD"; break;
	}

	wprintf(L"# gameRun: %s # SessionID: %d / X: %d / Y: %d\n", dirStr, Getsid(), x, y);
}

short Player::TakeDamage(int attackid, short damage)
{

	if (hp <= 0) return 0;

	hp -= damage;
	if (hp < 0) hp = 0;

	return hp;
}
