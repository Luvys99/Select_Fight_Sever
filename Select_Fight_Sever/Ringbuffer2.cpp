#include "Ringbuffer.h"

// 버퍼 포인터로 외부에서 쓸 수 있는 길이.
// (리턴 값) 사용가능 용량.
int RingBuffer::DirectEnqueueSize(void)
{
	// 논리적으로 남은 공간 크기 
	int freesize = GetFreeSize();

	// 물리적으로 남은 공간 크기
	int directsize = 0;
	// r_rear가 한 바퀴 돌아서 버퍼 앞쪽에 위치한 경우
	if (r_rear < r_front) // 물리적으로 남은 공간 크기값
	{
		directsize = r_front - r_rear;
	}
	else // 물리적으로 버퍼 끝까지 비어 있는 공간이 한 번에 읽고 쓰고 읽을 수 있는 길이
	{
		directsize = r_bufsize - r_rear;
	}

	// 둘 중 더 작은 값을 리턴 논리적인 공간을 고려한 크기와 비교했을 때
	// 더 작다면 그건 배열 끝에 도달해서 제한된 값이기 떄문에 해당 사이즈를 반환
	if (freesize < directsize)
	{
		return freesize;
	}
	
	return directsize;
}

// 외부에서 한 방에 읽을 수 있는 길이
// (리턴 값) 사용가능 용량.
int RingBuffer::DirectDequeueSize(void)
{
	// 논리적으로 사용하고 있는 공간
	int usesize = GetUseSize();

	// 물리적으로 한 번에 읽을 수 있는 크기
	int directsize = 0;
	// 실제로 한 번에 읽을 수 있는 공간이 버퍼 끝에 걸렸을 때
	if (r_rear < r_front)
	{
		directsize = r_bufsize - r_front;
	}
	else // 버퍼 끝에 걸리지 않고 한 번에 읽을 수 있을 때
	{
		directsize = r_rear - r_front;
	}

	if (usesize < directsize)
	{
		return usesize;
	}

	return directsize;
}

// 버퍼의 RearPos 포인터 얻음.
char* RingBuffer::GetRearBufferPtr(void)
{
	return &r_buffer[r_rear];
}

// 버퍼의 Front 포인터 얻음.
char* RingBuffer::GetFrontBufferPtr(void)
{
	return &r_buffer[r_front];
}

// 직접 링버퍼에 쓴 데이터 크기만큼 rear 이동 ( 인자로 이동한 크기 전달됨 )
// ( 리턴 값 이동한 크기 )
int RingBuffer::MoveRear(int iSize)
{
	r_rear = (r_rear + iSize) % r_bufsize;
	return iSize;
}

// 링버퍼에서 빼낸 바이트 만큼 front 이동 ( iSize = 빼는 바이트 크기 )
// ( 리턴 값 이동한 크기 )
int RingBuffer::MoveFront(int iSize)
{
	r_front = (r_front + iSize) % r_bufsize;
	return iSize;
}