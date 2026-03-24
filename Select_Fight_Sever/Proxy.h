#pragma once
#include "Serialize_Buffer.h"

inline void proxy_create_my_character(CMessage& msg, int id, char dir, short x, short y, char hp)
{
	PACKET_HEADER header;
	header.h_code = 0x89;
	header.h_size = sizeof(SC_CREATE_MY_CHARACTER);
	header.h_type = dfPACKET_SC_CREATE_MY_CHARACTER;

	msg.PutData((char*)&header, sizeof(PACKET_HEADER));
	msg  << id << dir << x << y << hp;
}

inline void proxy_create_my_other_character(CMessage& msg, int id, char dir, short x, short y, char hp)
{
	PACKET_HEADER header;
	header.h_code = 0x89;
	header.h_size = sizeof(SC_CREATE_OTHER_CHARACTER);
	header.h_type = dfPACKET_SC_CREATE_OTHER_CHARACTER;

	msg.PutData((char*)&header, sizeof(PACKET_HEADER));
	msg  << id << dir << x << y << hp;
}

inline void make_create_my_other_character(CMessage& msg, int id, char dir, short x, short y, char hp)
{
	PACKET_HEADER header;
	header.h_code = 0x89;
	header.h_size = sizeof(SC_CREATE_OTHER_CHARACTER);
	header.h_type = dfPACKET_SC_CREATE_OTHER_CHARACTER;

	msg.PutData((char*)&header, sizeof(PACKET_HEADER));
	msg  << id << dir << x << y << hp;
}

inline void proxy_delete_character(CMessage& msg, int id)
{
	PACKET_HEADER header;
	header.h_code = 0x89;
	header.h_size = sizeof(SC_DELETE_CHARACTER);
	header.h_type = dfPACKET_SC_DELETE_CHARACTER;

	msg.PutData((char*)&header, sizeof(PACKET_HEADER));
	msg  << id;
}

inline void proxy_move_start(CMessage& msg, int id, char dir, short x, short y)
{
	PACKET_HEADER header;
	header.h_code = 0x89;
	header.h_size = sizeof(SC_MOVE_START);
	header.h_type = dfPACKET_SC_MOVE_START;

	msg.PutData((char*)&header, sizeof(PACKET_HEADER));
	msg  << id << dir << x << y;
}

inline void proxy_move_stop(CMessage& msg, int id, char dir, short x, short y)
{
	PACKET_HEADER header;
	header.h_code = 0x89;
	header.h_size = sizeof(SC_MOVE_STOP);
	header.h_type = dfPACKET_SC_MOVE_STOP;

	msg.PutData((char*)&header, sizeof(PACKET_HEADER));
	msg  << id << dir << x << y;
}

inline void proxy_attack1(CMessage& msg, int id, char dir, short x, short y)
{
	PACKET_HEADER header;
	header.h_code = 0x89;
	header.h_size = sizeof(SC_ATTACK1);
	header.h_type = dfPACKET_SC_ATTACK1;

	msg.PutData((char*)&header, sizeof(PACKET_HEADER));
	msg  << id << dir << x << y;
}

inline void proxy_attack2(CMessage& msg, int id, char dir, short x, short y)
{
	PACKET_HEADER header;
	header.h_code = 0x89;
	header.h_size = sizeof(SC_ATTACK2);
	header.h_type = dfPACKET_SC_ATTACK2;

	msg.PutData((char*)&header, sizeof(PACKET_HEADER));
	msg  << id << dir << x << y;
}

inline void proxy_attack3(CMessage& msg, int id, char dir, short x, short y)
{
	PACKET_HEADER header;
	header.h_code = 0x89;
	header.h_size = sizeof(SC_ATTACK3);
	header.h_type = dfPACKET_SC_ATTACK3;

	msg.PutData((char*)&header, sizeof(PACKET_HEADER));
	msg  << id << dir << x << y;
}

inline void proxy_damage(CMessage& msg, int attackt_id, int damage_id, char hp)
{
	PACKET_HEADER header;
	header.h_code = 0x89;
	header.h_size = sizeof(SC_DAMAGE);
	header.h_type = dfPACKET_SC_DAMAGE;

	msg.PutData((char*)&header, sizeof(PACKET_HEADER));
	msg  << attackt_id << damage_id << hp;
}

