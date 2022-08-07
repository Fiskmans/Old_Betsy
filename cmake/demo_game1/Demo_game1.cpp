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


	engine::AssetManager& assetManager = engine::AssetManager::GetInstance();
	scene.SetMainCamera(&myCamera);
	

	myEnvironmentLight.myTexture = assetManager.GetCubeTexture("demo_game1/skybox.dds");
	scene.SetEnvironmentLight(&myEnvironmentLight);

	engine::AssetHandle modelHandle = assetManager.GetModel("Quaternius/Medieval/Buildings/Bell_Tower.fbx");
	myModel = std::make_unique<engine::ModelInstance>(modelHandle);
	
	scene.AddToScene(myModel.get());
}

void Demo_game1::Update()
{
	myCamera.ForceRedraw();
}

void Demo_game1::PrepareRender()
{
}
