#ifndef DEMO_GAME1_DEMO_GAME1_H
#define DEMO_GAME1_DEMO_GAME1_H

#include "engine/GameEngine.h"

#include "engine/graphics/Camera.h"
#include "engine/graphics/Environmentlight.h"

#include "engine/assets/ModelInstance.h" 

class Demo_game1 final : public engine::Game
{
public:
	Demo_game1();

	std::string AsciiName() final { return "Demo game"; }
	std::wstring Name() final { return L"Demo game"; }

	void Setup() final;
	void Update() final;
	void PrepareRender() final;
	void ImGui() final;
	virtual std::vector<std::pair<std::reference_wrapper<fisk::input::Action>, std::string>> GetActions() final;

private:

	void SetupActionsDefaultBindings();
	void InputImgui();
	engine::PerspectiveCamera myCamera;
	engine::graphics::EnvironmentLight myEnvironmentLight;
	std::unique_ptr<engine::ModelInstance> myModel;
};

#endif