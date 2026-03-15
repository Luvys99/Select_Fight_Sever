#pragma once
#include "NetStd.h"

class SetSelect
{ 
public:

	bool SelectFunc(SOCKET listensock, const vector<SOCKET>& readsocks, const vector<SOCKET>& writesocks);
	
	FD_SET* Getwset() { return &wset; }
	FD_SET* Getrset() { return &rset; }

private:
	FD_SET rset;
	FD_SET wset;

};