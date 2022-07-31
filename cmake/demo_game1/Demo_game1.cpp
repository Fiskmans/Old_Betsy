#include "Demo_game1.h"

#include "engine/assets/AssetManager.h"

#include "engine/SettingsManager.h"

#include "tools/Literals.h"


using namespace tools::size_literals;

Demo_game1::Demo_game1()
	: myCamera(engine::GameEngine::GetInstance().GetMainScene(), 1_cm, 100_m, engine::SettingsManager::GetInstance().myFOV.Get())
{
}

void Demo_game1::Setup()
{
	engine::RenderScene& scene = engine::GameEngine::GetInstance().GetMainScene();

	myEnvironmentLight.myTexture = engine::AssetManager::GetInstance().GetCubeTexture("demo_game1/skybox.dds");

	scene.SetMainCamera(&myCamera);
	scene.SetEnvironmentLight(&myEnvironmentLight);
}

void Demo_game1::Update()
{
}

void Demo_game1::PrepareRender()
{
}
