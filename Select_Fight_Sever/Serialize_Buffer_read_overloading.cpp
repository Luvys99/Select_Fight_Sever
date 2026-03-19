#include "Serialize_Buffer.h"
#include <string.h>
#include <cassert>


CMessage& CMessage::operator >> (unsigned char& uchvalue)
{
	assert(GetUseDataSize() >= sizeof(unsigned char));

	uchvalue = *(unsigned char*)GetReadPtr();
	MoveReadPos(sizeof(unsigned char));

	return *this;

}

CMessage& CMessage::operator >> (char& chvalue)
{
	assert(GetUseDataSize() >= sizeof(char));

	chvalue = *(char*)GetReadPtr();
	MoveReadPos(sizeof(char));

	return *this;

}

CMessage& CMessage::operator >> (unsigned short& ushvalue)
{
	assert(GetUseDataSize() >= sizeof(unsigned short));

	ushvalue = *(unsigned short*)GetReadPtr();
	MoveReadPos(sizeof(unsigned short));

	return *this;
}

CMessage& CMessage::operator >> (short& shvalue)
{
	assert(GetUseDataSize() >= sizeof(short));

	shvalue = *(short*)GetReadPtr();
	MoveReadPos(sizeof(short));

	return *this;
}

CMessage& CMessage::operator >> (int& ivalue)
{
	assert(GetUseDataSize() >= sizeof(int));

	ivalue = *(int*)GetReadPtr();
	MoveReadPos(sizeof(int));

	return *this;
}

CMessage& CMessage::operator >> (long& lvalue)
{
	assert(GetUseDataSize() >= sizeof(long));

	lvalue = *(long*)GetReadPtr();
	MoveReadPos(sizeof(long));

	return *this;
}

CMessage& CMessage::operator >> (float& fvalue)
{
	assert(GetUseDataSize() >= sizeof(float));

	fvalue = *(float*)GetReadPtr();
	MoveReadPos(sizeof(float));

	return *this;
}

CMessage& CMessage::operator >> (__int64& __ivalue)
{
	assert(GetUseDataSize() >= sizeof(__int64));

	__ivalue = *(__int64*)GetReadPtr();
	MoveReadPos(sizeof(__int64));

	return *this;
}

CMessage& CMessage::operator >> (double& dvalue)
{
	assert(GetUseDataSize() >= sizeof(double));

	dvalue = *(double*)GetReadPtr();
	MoveReadPos(sizeof(double));

	return *this;
}