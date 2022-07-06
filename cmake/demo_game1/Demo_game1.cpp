#include "Demo_game1.h"

#include "engine/SettingsManager.h"

#include "tools/Literals.h"


using namespace tools::size_literals;

Demo_game1::Demo_game1()
	: myCamera(1_cm, 100_m, engine::SettingsManager::GetInstance().myFOV.Get())
{
}

void Demo_game1::Setup()
{
	engine::GameEngine::GetInstance().GetMainScene().SetMainCamera(&myCamera);
}

void Demo_game1::Update()
{
}

void Demo_game1::PrepareRender()
{
}
