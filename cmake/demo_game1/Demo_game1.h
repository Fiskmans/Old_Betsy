#ifndef DEMO_GAME1_DEMO_GAME1_H
#define DEMO_GAME1_DEMO_GAME1_H

#include "engine/GameEngine.h"

#include "engine/graphics/Camera.h"
#include "engine/graphics/Environmentlight.h"

#include "engine/assets/ModelInstance.h" 

#include "tools/Literals.h"

using namespace tools::size_literals;
using namespace tools::rotation_literals;

class Demo_game1 final 
	: public engine::Game
	, public engine::ImGuiWindow
{
public:
	Demo_game1();

	std::string AsciiName() final { return "Demo game"; }
	std::wstring Name() final { return L"Demo game"; }

	void Setup() final;
	void Update() final;
	void PrepareRender() final;
	virtual std::vector<std::pair<std::reference_wrapper<fisk::input::Action>, std::string>> GetActions() final;

	void OnImgui() override;
	inline const char* ImGuiName() override { return "demo_game"; };

private:

	bool myRotate = true;
	float myFov = 10_deg;
	float myRotation = 0_deg;

	void SetupActionsDefaultBindings();
	void InputImgui();
	engine::PerspectiveCamera myCamera;
	engine::graphics::EnvironmentLight myEnvironmentLight;
	std::unique_ptr<engine::ModelInstance> myModel;

};

#endif