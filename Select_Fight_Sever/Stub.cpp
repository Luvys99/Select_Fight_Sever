#include "Stub.h"
#include "PacketDefine.h"

void ProcessMsg(InterfacePacketHandler* handler, int playeridx, unsigned char type, CMessage& msg)
{
	switch(type)
	{
	case dfPACKET_CS_MOVE_START:
	{
		char dir; short x; short y; 
		msg >> dir >> x >> y ;
		handler->ProcessMoveStart(playeridx, dir, x, y);
		break;
	}
	case dfPACKET_CS_MOVE_STOP:
	{
		char dir; short x; short y; 
		msg >> dir >> x >> y ;
		handler->ProcessMoveStop(playeridx, dir, x, y);
		break;
	}
	case dfPACKET_CS_ATTACK1:
	{
		char dir; short x; short y; 
		msg >> dir >> x >> y ;
		handler->ProcessAttack1(playeridx, dir, x, y);
		break;
	}
	case dfPACKET_CS_ATTACK2:
	{
		char dir; short x; short y; 
		msg >> dir >> x >> y ;
		handler->ProcessAttack2(playeridx, dir, x, y);
		break;
	}
	case dfPACKET_CS_ATTACK3:
	{
		char dir; short x; short y; 
		msg >> dir >> x >> y ;
		handler->ProcessAttack3(playeridx, dir, x, y);
		break;
	}
	default:
		break;
	}
}
