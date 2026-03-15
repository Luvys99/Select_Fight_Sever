#pragma once
#include <stdio.h>

class RingBuffer
{
private:
	char* r_buffer;
	int r_rear;
	int r_front;
	int r_bufsize;
public:
	RingBuffer() : r_rear(0), r_front(0), r_bufsize(4096)
	{
		r_buffer = new char[4096];
		if (r_buffer == nullptr)
		{
			wprintf(L"new alloc failed\n");
			return;
		}
	}
	RingBuffer(int size) :r_rear(0), r_front(0), r_bufsize(size)
	{
		r_buffer = new char[size];
		if (r_buffer == nullptr)
		{
			wprintf(L"new alloc failed\n");
			return;
		}
	}

	int GetBufferSize(void);
	int GetUseSize(void);
	int GetFreeSize(void);

	int Enqueue(const char* chpData, int iSize);
	int Dequeue(char* chpDest, int iSize);
	int Peek(char* chpDest, int iSize);

	void ClearBuffer(void);

	int DirectEnqueueSize(void);
	int DirectDequeueSize(void);

	int MoveRear(int iSize);
	int MoveFront(int iSize);

	char* GetFrontBufferPtr(void);
	char* GetRearBufferPtr(void);

};