
#include "engine/GameEngine.h"
#include "engine/ImguiManager.h"
#include "engine/SettingsManager.h"
#include "engine/Time.h"

#include "engine/utilities/StopWatch.h"

#include "engine/graphics/GraphicEngine.h"
#include "engine/graphics/WindowManager.h"

#include "engine/assets/AssetManager.h"

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
			ImGuiManager::GetInstance().Init();
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

	//void GameEngine::DevicesImgui()
	//{
	//	for (const std::shared_ptr<fisk::input::InputDevice>& device : myInput.GetDevices())
	//	{
	//		if (ImGui::TreeNode(device->GetName().c_str()))
	//		{
	//			if (ImGui::BeginTable("channels", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_NoHostExtendX))
	//			{
	//				ImGui::TableSetupColumn("name", ImGuiTableColumnFlags_WidthFixed);
	//				ImGui::TableSetupColumn("value", ImGuiTableColumnFlags_WidthFixed);
	//				ImGui::TableHeadersRow();
	//
	//				for (const std::unique_ptr<fisk::input::InputDevice::Channel>& channel : device->GetChannels())
	//				{
	//					ImGui::TableNextRow();
	//					ImGui::TableNextColumn();
	//					ImGui::Text(channel->myName.c_str());
	//					ImGui::TableNextColumn();
	//					old_betsy_imgui::FloatIndicator(channel->myCurrentValue);
	//				}
	//				ImGui::EndTable();
	//			}
	//
	//			ImGui::TreePop();
	//		}
	//
	//	}
	//}

	void GameEngine::Run()
	{
		while (!myGame->WantsExit())
		{
			Update();

			myGame->Update();
			myGame->PrepareRender();

			graphics::GraphicsEngine::GetInstance().RenderFrame();

			ImGuiManager::GetInstance().Update();

			graphics::GraphicsEngine::GetInstance().EndFrame();
		}
	}

	void GameEngine::Update()
	{
		Time::GetInstance().Update();
		WindowManager::GetInstance().Update();
		myInput.Update();
	}

	void 
	GameEngine::OnImgui()
	{
		static bool demo = false;
		
		ImGui::Checkbox("Demo Window", &demo);
		if (demo)
			ImGui::ShowDemoWindow(&demo);
	}
	
}

