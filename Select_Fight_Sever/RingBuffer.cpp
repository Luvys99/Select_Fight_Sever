#include "Ringbuffer.h"
#include <string.h>

// 버퍼 사이즈 얻기
int RingBuffer::GetBufferSize(void)
{
	return r_bufsize;
}

// 사용중인 버퍼 총 용량 얻기
int RingBuffer::GetUseSize(void)
{
	//r_rear가 한 바퀴 돌아서 값이 작아졌을 때
	if (r_rear < r_front)
	{
		return (r_rear + r_bufsize) - r_front;
	}
	else
	{
		return r_rear - r_front;
	}
}

// 비어있는 버퍼 총 용량 얻기
int RingBuffer::GetFreeSize(void)
{
	return r_bufsize - GetUseSize();
}

// TCP 수신버퍼에서 recv한 페이로드 링 버퍼에 저장
int RingBuffer::Enqueue(const char* chpData, int iSize)
{
	// 방어 코드 : 넣으려는 데이터 보다 링 버퍼 공간이 작으면 종료
	if (GetFreeSize() < iSize)
	{
		wprintf(L"RingBuffer Enqueue Failed\n");
		return -1;
	}

	// 버퍼 끝까지의 공간 
	int endbufsize = r_bufsize - r_rear;

	// 버퍼 끝에 남은 공간보다 저장할 데이터가 큰 경우 ( memcpy 2번에 나눠서 저장 )
	if (endbufsize < iSize)
	{
		// 남은 공간까지 복사
		memcpy(r_buffer + r_rear, chpData, endbufsize);
		// 나머지 데이터를 r_buffer 앞공간부터 복사
		memcpy(r_buffer, chpData + endbufsize, iSize - endbufsize);
	}
	else
	{
		memcpy(r_buffer + r_rear, chpData, iSize);
	}

	// 데이터 복사 완료 후에 r_rear 갱신
	r_rear = (r_rear + iSize) % r_bufsize;

	return iSize; // 엔큐한 크기만큼 리턴
}

// 링 버퍼에서 데이터를 꺼냄
int RingBuffer::Dequeue(char* chpDest, int iSize)
{
	// 방어 코드 : 빼내려는 데이터보다 링버퍼에 들어있는 데이터가 작으면 종료
	if (GetUseSize() < iSize)
	{
		wprintf(L"RingBuffer Dequeue Failed\n");
		return -1;
	}

	// 버퍼 끝까지의 데이터 크기
	int enddatasize = r_bufsize - r_front;

	//꺼낼 데이터가 버퍼 끝에 걸려있다면
	if (enddatasize < iSize)
	{
		// 버퍼 끝까지의 데이터 복사
		memcpy(chpDest, r_buffer + r_front, enddatasize);

		// 나머지 데이터 복사
		memcpy(chpDest + enddatasize, r_buffer, iSize - enddatasize);
	}
	else // 꺼낼 데이터가 버퍼 끝에 안 걸려있으면
	{
		memcpy(chpDest, r_buffer + r_front, iSize);
	}

	// 복사 끝내고 r_front 갱신
	r_front = (r_front + iSize) % r_bufsize;

	return iSize;
}

// 데이터가 제대로 저장되었는지 확인 ( 리턴 값 : 저장된 바이트 수 )
int RingBuffer::Peek(char* chpDest, int iSize)
{
	// 버퍼 끝까지의 데이터 크기
	int enddatasize = r_bufsize - r_front;

	//꺼낼 데이터가 버퍼 끝에 걸려있다면
	if (enddatasize < iSize)
	{
		// 버퍼 끝까지의 데이터 복사
		memcpy(chpDest, r_buffer + r_front, enddatasize);

		// 나머지 데이터 복사
		memcpy(chpDest + enddatasize, r_buffer, iSize - enddatasize);
	}
	else // 꺼낼 데이터가 버퍼 끝에 안 걸려있으면
	{
		memcpy(chpDest, r_buffer + r_front, iSize);
	}

	return iSize;
}

void RingBuffer::ClearBuffer(void)
{
	r_rear = 0;
	r_front = 0;
}