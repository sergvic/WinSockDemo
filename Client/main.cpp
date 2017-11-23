#include <WinSock2.h>
#include "..\SockUtils.h"

void main()
{
	InitSocketsLib();

	TestSendToServer();
	//TestSockets();

	ReleaseSocketsLib();
}