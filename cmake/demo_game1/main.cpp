
#include "versionConfig.h"

#include "tools/Logger.h"

#include "tools/Time.h"
#include "tools/SubsystemManager.h"
#include "tools/NameThread.h"

#include "engine/GameEngine.h"

#include "demo_game1/Demo_game1.h"

#include <iostream>
#include <vector>
#include <cassert>

int main(int argc, char** argv)
{
	tools::NameThread("main_thread");

	fisk::tools::SetFilter(fisk::tools::Type::All & ~fisk::tools::Type::AnyVerbose);
	fisk::tools::SetHalting(fisk::tools::Type::AnyError | fisk::tools::Type::SystemCrash);

	fisk::tools::SetColor(fisk::tools::Type::AnyError, FOREGROUND_RED | FOREGROUND_INTENSITY);
	fisk::tools::SetColor(fisk::tools::Type::AnyWarning, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	fisk::tools::SetColor(fisk::tools::Type::AnyInfo, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	fisk::tools::SetColor(fisk::tools::Type::SystemNetwork, FOREGROUND_BLUE | FOREGROUND_INTENSITY);

	tools::SubsystemManager::GetInstance().AddSubsystemVersion("engine", VERSIONSTRING(ENGINE));
	tools::SubsystemManager::GetInstance().AddSubsystemVersion("imgui", VERSIONSTRING(IMGUI));
	tools::SubsystemManager::GetInstance().AddSubsystemVersion("tools", VERSIONSTRING(TOOLS));

	tools::SubsystemManager::GetInstance().LogAllVersions();

	Demo_game1 game;

	engine::GameEngine::GetInstance().Init(game);
	engine::GameEngine::GetInstance().RunGame();

	fisk::tools::Shutdown();
}
