#pragma once
#include <stdio.h>


#ifndef __PACKET__
#define __PACKET__

class CMessage
{
	enum en_PACKET
	{
		BUFFER_DEFAULT = 1400
	};

public:
	CMessage() : buffer_size(BUFFER_DEFAULT), use_size(0), rear(0), front(0)
	{
		s_buffer = new char[BUFFER_DEFAULT];
		if (s_buffer == nullptr)
		{
			wprintf(L"buffer allocate failed!!\n");
			return;
		}
	}
	CMessage(int size) : buffer_size(size), use_size(0), rear(0), front(0)
	{
		s_buffer = new char[size];
		if (s_buffer == nullptr)
		{
			wprintf(L"buffer allocate failed!!\n");
			return;
		}
	}
	virtual ~CMessage()
	{
		delete[]s_buffer;
	}

	int GetBufferSize() { return buffer_size; }
	int GetUseDataSize() 
	{ 
		return rear - front;
	}
	char* GetBufferPtr() { return s_buffer; }
	char* GetWritePtr() { return &s_buffer[rear]; }
	char* GetReadPtr() { return &s_buffer[front]; }
	// 직렬화 하기 위해 집어 넣은 데이터의 크기를 확인하기 위함
	int MoveWritePos(int size)
	{
		rear += size;
		return size;
	}
	// 역직렬화 할 때 어디 까지 데이터를 꺼냈는지 확인하기 위함
	int MoveReadPos(int size)
	{
		front += size;
		return size;
	}
	void Clear() { rear = 0, front = 0; }

	// 데이터 얻기
	int GetData(char* chpDest, int isize);
	// 데이터 삽입
	int PutData(char* chpSrc, int isize);


	// 연산자 오버로딩 대입이 발생하지 않게 차단!
	CMessage& operator = (CMessage& SrcMessage) = delete;

	// 연산자 오버로딩 Write 함수
	CMessage& operator << (unsigned char uchvalue);
	CMessage& operator << (char chvalue);

	CMessage& operator << (unsigned short ushvalue);
	CMessage& operator << (short shvalue);

	CMessage& operator << (int ivalue);
	CMessage& operator << (long lvalue);
	CMessage& operator << (float fvalue);

	CMessage& operator << (__int64 __ivalue);
	CMessage& operator << (double dvalue);


	// 연산자 오버로딩 Read 함수
	CMessage& operator >> (unsigned char& uchvalue);
	CMessage& operator >> (char& chvalue);

	CMessage& operator >> (unsigned short& ushvalue);
	CMessage& operator >> (short& shvalue);

	CMessage& operator >> (int& ivalue);
	CMessage& operator >> (long& lvalue);
	CMessage& operator >> (float& fvalue);

	CMessage& operator >> (__int64& __ivalue);
	CMessage& operator >> (double& dvalue);

private:
	int buffer_size;
	int use_size;
	char* s_buffer;
	int rear;
	int front;
};

#endif