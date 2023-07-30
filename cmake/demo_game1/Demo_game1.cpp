#include "Demo_game1.h"

#include "common/Macros.h"

#include "engine/assets/AssetManager.h"
#include "engine/assets/ModelInstance.h"

#include "engine/SettingsManager.h"
#include "engine/Time.h"

#include "imgui/WindowControl.h"

#include "Actions.h"

Demo_game1::Demo_game1()
	: ImGuiWindow("Game")
	, myCamera(engine::GameEngine::GetInstance().GetMainScene(), 1_m, 100_m, myFov)
{
}

void Demo_game1::Setup()
{
	SetupActionsDefaultBindings();

	engine::RenderScene& scene = engine::GameEngine::GetInstance().GetMainScene();

	engine::AssetManager& assetManager = engine::AssetManager::GetInstance();
	scene.SetMainCamera(&myCamera);


	myEnvironmentLight.myTexture = assetManager.GetCubeTexture("demo_game1/skybox.dds");
	scene.SetEnvironmentLight(&myEnvironmentLight);

	engine::AssetHandle<engine::ModelAsset> modelHandle = assetManager.GetModel("Quaternius/Medieval/Buildings/Bell_Tower.fbx");
	myModel = std::make_unique<engine::ModelInstance>(modelHandle);

	scene.AddToScene(myModel.get());

	scene.GetMainCamera()->Move(tools::V3f(50_m,0, 0));
	//scene.GetMainCamera()->LookAt(tools::V3f(0, 0, 0));
}

std::vector<std::pair<std::reference_wrapper<fisk::input::Action>, std::string>> Demo_game1::GetActions()
{
	return {
		{ Actions.Jump, "jump" },
		{ Actions.Dig, "dig" }
	};
}

void Demo_game1::SetupActionsDefaultBindings()
{
	Actions.Jump.myWantedChannels.push_back("keyboard.space");
	Actions.Dig.myWantedChannels.push_back("mouse.left");
	Actions.Dig.myWantedChannels.push_back("keyboard.d");
}

void Demo_game1::InputImgui()
{
}

void Demo_game1::OnImgui()
{
	if (ImGui::TreeNode("Actions"))
	{
		class ActionImgui
		{
		public:
			ActionImgui(fisk::input::DigitalAction& aAction, const char* aName)
				: myAction(aAction)
				, myName(aName)
			{
				myPressed = myAction.OnPressed.Register([&]()
				{
					myPressTime = std::chrono::steady_clock::now();
				});
				myReleased = myAction.OnReleased.Register([&]()
				{
					myReleaseTime = std::chrono::steady_clock::now();
				});
			}

			void Imgui()
			{
				ImGui::Text(myName);

				std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
				float pressIntens = 1.f - std::min(1.f, std::chrono::duration_cast<std::chrono::milliseconds>(now - myPressTime).count() / 500.f);
				float releaseintens = 1.f - std::min(1.f, std::chrono::duration_cast<std::chrono::milliseconds>(now - myReleaseTime).count() / 500.f);

				ImGui::ColorButton("hold", SelectColor(myAction.IsHeld() ? 1.f : 0.f), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip);
				ImGui::SameLine();
				ImGui::ColorButton("press", SelectColor(pressIntens), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip);
				ImGui::SameLine();
				ImGui::ColorButton("release", SelectColor(releaseintens), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip);
				ImGui::Text("Mapped to: %s", myAction.myBoundTo.c_str());

				ImGui::Text("Wanted channels: ");
				bool first = true;
				for (const std::string& channel : myAction.myWantedChannels)
				{
					if (!first)
					{
						ImGui::SameLine();
						ImGui::Text(",");
					}

					ImGui::SameLine();
					ImGui::TextColored(channel == myAction.myBoundTo ? ImVec4(1.f, 1.f, 1.f, 1.f) : ImVec4(0.6f, 0.6f, 0.6f, 1.f), channel.c_str());

					first = true;
				}

			}

		private:

			ImVec4 SelectColor(float aValue)
			{
				return ImVec4(1.f - aValue, aValue, 0.f, 1.f);
			}

			fisk::input::DigitalAction& myAction;
			const char* myName;
			std::chrono::steady_clock::time_point myPressTime;
			std::chrono::steady_clock::time_point myReleaseTime;

			fisk::tools::EventReg myPressed;
			fisk::tools::EventReg myReleased;
		};

		static ActionImgui actionList[] =
		{
			{ Actions.Dig, "Dig" },
			{ Actions.Jump, "Jump" }
		};

		for (ActionImgui& action : actionList)
		{
			ImGui::Separator();
			action.Imgui();
		}
		ImGui::TreePop();
		ImGui::Separator();
	}

	if (ImGui::SliderFloat("Fov", &myFov, 10_deg, 170_deg))
	{
		myCamera.SetFOV(myFov);
	}

	ImGui::Checkbox("Rotation", &myRotate);
	if (!myRotate)
	{
		if (ImGui::SliderFloat("Angle", &myRotation, 0_deg, 360_deg))
		{
			myModel->SetRotation(tools::V3f(0, myRotation, 0));
		}
	}
}

void Demo_game1::Update()
{
	if (myRotate)
	{
		myModel->Rotate(tools::V3f(0, 1 * engine::Time::DeltaTime().count(), 0));
	}
}

void Demo_game1::PrepareRender()
{

}
