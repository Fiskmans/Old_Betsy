#include <pch.h>
#include <iostream>
#include <GameServer.h>

int main()
{
    GameServer server;
    server.StartServer();
    std::cout << "Goodbye!\n";
}