#include "Serialize_Buffer.h"
#include <string.h>
#include <cassert>

// 데이터 얻기
int CMessage::GetData(char* chpDest, int isize)
{
	// 빼내려는 데이터 크기보다 버퍼 안의 데이터 크기가 작으면 프로그램 중단
	assert(GetUseDataSize() >= isize);
	if (GetUseDataSize() < isize) return -1;

	memcpy(chpDest, GetReadPtr(), isize);
	MoveReadPos(isize);

	return isize;
}

// 데이터 삽입 ( 가변 크기 때문에 문자열과 문자열의 복사는 memcpy를 사용해야 됨 )
// 헤더 삽입에도 사용
int CMessage::PutData(char* chpSrc, int isize)
{
	// 버퍼 사이즈를 초과하면 함수 종료
	assert(isize + GetUseDataSize() <= buffer_size);
	if (GetUseDataSize() + isize > buffer_size) return -1;

	memcpy(GetWritePtr(), chpSrc, isize);
	MoveWritePos(isize);

	return isize;
}

CMessage& CMessage::operator << (unsigned char uchvalue)
{
	// 버퍼에 넣을 공간이 있으면 통과! 없으면 프로그램 중단
	assert(GetUseDataSize() + sizeof(unsigned char) <= buffer_size);

	*(unsigned char*)GetWritePtr() = uchvalue;

	//memcpy(GetWritePtr(), &uchvalue, sizeof(unsigned char));
	MoveWritePos(sizeof(unsigned char));

	return *this;
}

CMessage& CMessage::operator << (char chvalue)
{
	// 버퍼에 넣을 공간이 있으면 통과! 없으면 프로그램 중단
	assert(GetUseDataSize() + sizeof(char) <= buffer_size);

	*GetWritePtr() = chvalue;

	//memcpy(GetWritePtr(), &chvalue, sizeof(char));
	MoveWritePos(sizeof(char));

	return *this;
}

CMessage& CMessage::operator << (unsigned short ushvalue)
{
	// 버퍼에 넣을 공간이 있으면 통과! 없으면 프로그램 중단
	assert(GetUseDataSize() + sizeof(unsigned short) <= buffer_size);

	*(unsigned short*)GetWritePtr() = ushvalue;

	//memcpy(GetWritePtr(), &ushvalue, sizeof(unsigned short));
	MoveWritePos(sizeof(unsigned short));

	return *this;
}
CMessage& CMessage::operator << (short shvalue)
{
	// 버퍼에 넣을 공간이 있으면 통과! 없으면 프로그램 중단
	assert(GetUseDataSize() + sizeof(short) <= buffer_size);

	*(short*)GetWritePtr() = shvalue;

	//memcpy(GetWritePtr(), &shvalue, sizeof(short));
	MoveWritePos(sizeof(short));

	return *this;
}

CMessage& CMessage::operator << (int ivalue)
{
	// 버퍼에 넣을 공간이 있으면 통과! 없으면 프로그램 중단
	assert(GetUseDataSize() + sizeof(int) <= buffer_size);

	*(int*)GetWritePtr() = ivalue;

	//memcpy(GetWritePtr(), &ivalue, sizeof(int));
	MoveWritePos(sizeof(int));

	return *this;
}
CMessage& CMessage::operator << (long lvalue)
{
	// 버퍼에 넣을 공간이 있으면 통과! 없으면 프로그램 중단
	assert(GetUseDataSize() + sizeof(long) <= buffer_size);

	*(long*)GetWritePtr() = lvalue;

	//memcpy(GetWritePtr(), &lvalue, sizeof(long));
	MoveWritePos(sizeof(long));

	return *this;
}
CMessage& CMessage::operator << (float fvalue)
{
	// 버퍼에 넣을 공간이 있으면 통과! 없으면 프로그램 중단
	assert(GetUseDataSize() + sizeof(float) <= buffer_size);

	*(float*)GetWritePtr() = fvalue;
	
	//memcpy(GetWritePtr(), &fvalue, sizeof(float));
	MoveWritePos(sizeof(float));

	return *this;
}

CMessage& CMessage::operator << (__int64 __ivalue)
{
	// 버퍼에 넣을 공간이 있으면 통과! 없으면 프로그램 중단
	assert(GetUseDataSize() + sizeof(__int64) <= buffer_size);

	*(__int64*)GetWritePtr() = __ivalue;

	//memcpy(GetWritePtr(), &__ivalue, sizeof(__int64));
	MoveWritePos(sizeof(__int64));

	return *this;
}
CMessage& CMessage::operator << (double dvalue)
{
	// 버퍼에 넣을 공간이 있으면 통과! 없으면 프로그램 중단
	assert(GetUseDataSize() + sizeof(double) <= buffer_size);

	*(double*)GetWritePtr() = dvalue;

	//memcpy(GetWritePtr(), &dvalue, sizeof(double));
	MoveWritePos(sizeof(double));

	return *this;
}
