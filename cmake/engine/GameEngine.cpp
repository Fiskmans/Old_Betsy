
#include "engine/GameEngine.h"
#include "engine/SettingsManager.h"
#include "engine/graphics/GraphicEngine.h"
#include "engine/graphics/WindowManager.h"

#include "engine/assets/AssetManager.h"

#include "engine/graph/NodeManager.h"
#include "engine/graph/NodeRegistration.h"
#include "engine/graph/Graph.h"

#include "imgui/CustomWidgets.h"
#include "imgui/WindowControl.h"
#include "imgui/backend/imgui_impl_dx11.h"
#include "imgui/backend/imgui_impl_win32.h"
#include "imgui/imgui.h"

#include "tools/ImGuiHelpers.h"
#include "tools/Stopwatch.h"
#include "tools/NameThread.h"

#include <algorithm>

namespace engine
{
	Game::Game()
	{
		LOG_SYS_INFO("Registering nodes");
		tools::Stopwatch stopWatch;
		{
			GameEngine::GetInstance().RegisterEngineNodes();
			RegisterNodes();
			graph::NodeManager::GetInstance().EndNode();
		}
		LOG_SYS_INFO("Nodes registerd in " + std::to_string(stopWatch.Read()) + " seconds");
	}

	void GameEngine::Init(Game& aGame)
	{
		LOG_SYS_INFO("Initializing Game Engine to run [" + aGame.AsciiName() + "]");
		
		myGame = &aGame;
		
		tools::Stopwatch stopWatch;
		{
			tools::TimedScope scopeTimer(stopWatch);

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

		LOG_SYS_INFO("Game Engine Initialization completed in " + std::to_string(stopWatch.Read()) + " seconds");
	}

	void GameEngine::RunGame()
	{
		LOG_SYS_INFO("Initializing Game");
		tools::Stopwatch setupWatch;
		{
			tools::TimedScope scopeTimer(setupWatch);
			myGame->Setup();
		}
		LOG_SYS_INFO("Game Initialization completed in " + std::to_string(setupWatch.Read()) + " seconds");

		Run();
	}

	void GameEngine::Imgui()
	{
		PERFORMANCETAG("Engine");

		old_betsy_imgui::WindowControl::DrawWindowControl();

		old_betsy_imgui::WindowControl::Window("Engine", [&]() { EngineImgui(); });
		old_betsy_imgui::WindowControl::Window("Devices", [&]() { DevicesImgui(); });
		old_betsy_imgui::WindowControl::Window("Performance", PerformanceWindow);
		old_betsy_imgui::WindowControl::Window("Scene", [&](){ SceneImgui(); });

		graph::NodeManager::GetInstance().Imgui();
		graph::GraphManager::GetInstance().Imgui();
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

	void GameEngine::PerformanceWindow()
	{
		static bool accumulate = true;
		ImGui::Checkbox("Accumulate", &accumulate);

		struct namedThread
		{
			std::thread::id myId;
			fisk::tools::TimeTree* myRoot;
			std::string myName;
			auto operator <=>(const namedThread& aOther) const { return myId <=> aOther.myId; }
		};
		
		std::vector<namedThread> sortedThreads;

		{
			fisk::tools::LockedResource<fisk::tools::RootCollection> handle = fisk::tools::AllRoots();
			for (std::pair<std::thread::id, fisk::tools::TimeTree*> threadTimeRoot : handle.Get())
			{
				namedThread thread;
				thread.myId = threadTimeRoot.first;
				thread.myRoot = threadTimeRoot.second;
				thread.myName = tools::GetNameOfThread(threadTimeRoot.first);

				sortedThreads.insert(std::upper_bound(sortedThreads.begin(), sortedThreads.end(), thread), thread);
			}

			if (sortedThreads.empty())
			{
				ImGui::Text("No performance data available");
				return;
			}

			static int selected = 0;
			if (selected >= sortedThreads.size())
			{
				selected = sortedThreads.size();
			}

			if (ImGui::BeginCombo("Thread", sortedThreads[selected].myName.c_str()))
			{
				for (size_t i = 0; i < sortedThreads.size(); i++)
					if (ImGui::Selectable(sortedThreads[selected].myName.c_str()))
						selected = i;

				ImGui::EndCombo();
			}


			tools::DrawTimeTree(sortedThreads[selected].myRoot);
		}

		if (!accumulate)
			fisk::tools::FlushTimeTree();
	}

	void GameEngine::RegisterEngineNodes()
	{
		LOG_SYS_INFO("Registering Engine nodes");
		tools::Stopwatch stopWatch;
		{
			tools::TimedScope scopeTimer(stopWatch);
			graph::NodeRegistration::Register();
		}
		LOG_SYS_INFO("Engine nodes registerd in " + std::to_string(stopWatch.Read()) + " seconds");
	}

	void GameEngine::Run()
	{
		while (!myGame->WantsExit())
		{
			PERFORMANCETAG("Main Update Loop");

			Update();
			{
				PERFORMANCETAG("Game Update");
				myGame->Update();
			}
			{
				PERFORMANCETAG("Game Prepare Render");
				myGame->PrepareRender();
			}
			
			{
				PERFORMANCETAG("Render frame");
				graphics::GraphicsEngine::GetInstance().RenderFrame();
			}

			{
				PERFORMANCETAG("Imgui");
				ImGui_ImplDX11_NewFrame();
				ImGui_ImplWin32_NewFrame();
				ImGui::NewFrame();

				{
					PERFORMANCETAG("Imgui Content");
					Imgui();
				}

				{
					PERFORMANCETAG("Imgui Render");
					ImGui::Render();
				}

				ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
			}

			{
				PERFORMANCETAG("End frame");
				graphics::GraphicsEngine::GetInstance().EndFrame();
			}
		}
	}

	void GameEngine::Update()
	{
		PERFORMANCETAG("Engine Update");
		WindowManager::GetInstance().Update();
		myInput.Update();
	}
	
}

