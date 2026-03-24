#pragma once
#include "Serialize_Buffer.h"


class InterfacePacketHandler
{
public:
	~InterfacePacketHandler() {}

	virtual void ProcessMoveStart (int playeridx, char dir, short x, short y) = 0;
	virtual void ProcessMoveStop (int playeridx, char dir, short x, short y) = 0;
	virtual void ProcessAttack1 (int playeridx, char dir, short x, short y) = 0;
	virtual void ProcessAttack2 (int playeridx, char dir, short x, short y) = 0;
	virtual void ProcessAttack3 (int playeridx, char dir, short x, short y) = 0;
};

void ProcessMsg(InterfacePacketHandler* handler, int playeridx, unsigned char type, CMessage& msg);
