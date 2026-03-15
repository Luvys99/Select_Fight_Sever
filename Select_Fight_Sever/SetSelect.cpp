#include "SetSelect.h"

bool SetSelect::SelectFunc(SOCKET listensock, const vector<SOCKET>& readsocks, const vector<SOCKET>& writesocks)
{
	FD_ZERO(&rset);
	FD_ZERO(&wset);

	// listen sock setting
	FD_SET(listensock, &rset);

	// Playersock recv setting
	for (SOCKET s : readsocks)
	{
		FD_SET(s, &rset);
	}

	// Playersock send setting
	for (SOCKET s : writesocks)
	{
		FD_SET(s, &wset);
	}

	// 프레임 로직을 돌려야 하기 때문에 select 함수 대기 시간 0으로 설정
	timeval tv{ 0, 0 };
	int select_ret = select(0, &rset, &wset, NULL, &tv);
	if (select_ret == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			wprintf(L"select failed error_code : %d\n", WSAGetLastError());
			return false;
		}
		
	}
	
	return true;
};

