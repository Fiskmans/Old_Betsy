#include <iostream>
#include <vector>
#include <cassert>

#define NOMINMAX
#include <windows.h>

#include "versionConfig.h"

#include "logger/Logger.h"

#include "tools/TimeHelper.h"
#include "tools/SubsystemManager.h"

#include "engine/GameEngine.h"

#include "demo_game1/Demo_game1.h"

int main(int argc, char** argv)
{
	logger::SetFilter(logger::Type::All & ~logger::Type::AnyVerbose);
	logger::SetHalting(logger::Type::AnyError | logger::Type::SystemCrash);

	logger::SetColor(logger::Type::AnyError,		FOREGROUND_RED |										FOREGROUND_INTENSITY);
	logger::SetColor(logger::Type::AnyWarning,		FOREGROUND_RED | FOREGROUND_GREEN |						FOREGROUND_INTENSITY);
	logger::SetColor(logger::Type::AnyInfo,			FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE |	FOREGROUND_INTENSITY);
	logger::SetColor(logger::Type::SystemNetwork,										FOREGROUND_BLUE |	FOREGROUND_INTENSITY);

	tools::SubsystemManager::GetInstance().AddSubsystemVersion("engine", VERSIONSTRING(ENGINE));
	tools::SubsystemManager::GetInstance().AddSubsystemVersion("imgui", VERSIONSTRING(IMGUI));
	tools::SubsystemManager::GetInstance().AddSubsystemVersion("launcher", VERSIONSTRING(LAUNCHER));
	tools::SubsystemManager::GetInstance().AddSubsystemVersion("logger", VERSIONSTRING(LOGGER));
	tools::SubsystemManager::GetInstance().AddSubsystemVersion("tools", VERSIONSTRING(TOOLS));

	tools::SubsystemManager::GetInstance().LogAllVersions();

	
	Demo_game1 game;

	engine::GameEngine::GetInstance().Init(game);
	engine::GameEngine::GetInstance().RunGame();

	logger::Shutdown();
}
