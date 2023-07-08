
#include "engine/GameEngine.h"
#include "engine/SettingsManager.h"
#include "engine/Time.h"

#include "engine/utilities/StopWatch.h"

#include "engine/graphics/GraphicEngine.h"
#include "engine/graphics/WindowManager.h"

#include "engine/assets/AssetManager.h"

#include "imgui/CustomWidgets.h"
#include "imgui/WindowControl.h"
#include "imgui/backend/imgui_impl_dx11.h"
#include "imgui/backend/imgui_impl_win32.h"
#include "imgui/imgui.h"

#include "tools/ImGuiHelpers.h"
#include "tools/NameThread.h"

#include <algorithm>

namespace engine
{
	Game::Game()
	{
	}

	void GameEngine::Init(Game& aGame)
	{
		LOG_SYS_INFO("Initializing Game Engine to run [" + aGame.AsciiName() + "]");
		
		myGame = &aGame;
		
		engine::utilities::StopWatch stopWatch;
		{
			AssetManager::GetInstance().Init("data/assets/", "baked/");

			graphics::GraphicsEngine::GetInstance().Init(SettingsManager::GetInstance().myWindowSize.Get());

#ifdef _DEBUG
			AssetManager::GetInstance().Preload();
#endif // _DEBUG

			IMGUI_CHECKVERSION();
			ImGui::CreateContext();

			SettingsManager::GetInstance().LoadOrDefaultImGuiStyle();
			ImGui_ImplDX11_Init(graphics::GraphicsEngine::GetInstance().GetFrameWork().GetDevice(), graphics::GraphicsEngine::GetInstance().GetFrameWork().GetContext());
			ImGui_ImplWin32_Init(WindowManager::GetInstance().GetWindowHandle());
		}

		for (auto& action : myGame->GetActions())
		{
			myInput.RegisterAction(action.first, action.second);
		}

		LOG_SYS_INFO("Game Engine Initialization completed in " + std::to_string(stopWatch.Stop().count()) + " seconds");
	}

	void GameEngine::RunGame()
	{
		LOG_SYS_INFO("Initializing Game");
		engine::utilities::StopWatch setupWatch;
		{
			myGame->Setup();
		}
		LOG_SYS_INFO("Game Initialization completed in " + std::to_string(setupWatch.Stop().count()) + " seconds");

		Run();
	}

	void GameEngine::Imgui()
	{
		old_betsy_imgui::WindowControl::DrawWindowControl();

		old_betsy_imgui::WindowControl::Window("Engine", [&]() { EngineImgui(); });
		old_betsy_imgui::WindowControl::Window("Devices", [&]() { DevicesImgui(); });
		old_betsy_imgui::WindowControl::Window("Scene", [&](){ SceneImgui(); });

		AssetManager::GetInstance().ImGui();

		myGame->ImGui();
	}

	void GameEngine::DevicesImgui()
	{
		for (const std::shared_ptr<fisk::input::InputDevice>& device : myInput.GetDevices())
		{
			if (ImGui::TreeNode(device->GetName().c_str()))
			{
				if (ImGui::BeginTable("channels", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_NoHostExtendX))
				{
					ImGui::TableSetupColumn("name", ImGuiTableColumnFlags_WidthFixed);
					ImGui::TableSetupColumn("value", ImGuiTableColumnFlags_WidthFixed);
					ImGui::TableHeadersRow();

					for (const std::unique_ptr<fisk::input::InputDevice::Channel>& channel : device->GetChannels())
					{
						ImGui::TableNextRow();
						ImGui::TableNextColumn();
						ImGui::Text(channel->myName.c_str());
						ImGui::TableNextColumn();
						old_betsy_imgui::FloatIndicator(channel->myCurrentValue);
					}
					ImGui::EndTable();
				}

				ImGui::TreePop();
			}

		}
	}

	void GameEngine::EngineImgui()
	{
		static bool demo = false;

		ImGui::Checkbox("Demo Window", &demo);
		if (demo)
			ImGui::ShowDemoWindow(&demo);
	}

	void GameEngine::SceneImgui()
	{
		myMainScene.ImGui();
	}

	void GameEngine::Run()
	{
		while (!myGame->WantsExit())
		{
			Update();

			myGame->Update();
			myGame->PrepareRender();

			graphics::GraphicsEngine::GetInstance().RenderFrame();

			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			Imgui();
			ImGui::Render();

			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			graphics::GraphicsEngine::GetInstance().EndFrame();
		}
	}

	void GameEngine::Update()
	{
		Time::GetInstance().Update();
		WindowManager::GetInstance().Update();
		myInput.Update();
	}
	
}

