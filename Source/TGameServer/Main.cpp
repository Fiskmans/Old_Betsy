#include <pch.h>
#include "ServerMain.h"


int main()
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_RED);
	CServerMain server;
	server.StartServer();
	system("pause");

	return 0;
}