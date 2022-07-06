
#include "engine/GameEngine.h"
#include "engine/SettingsManager.h"
#include "engine/graphics/GraphicEngine.h"
#include "engine/graphics/WindowManager.h"

#include "engine/assets/AssetManager.h"

#include "imgui/imgui.h"
#include "imgui/WindowControl.h"
#include "imgui/backend/imgui_impl_win32.h"
#include "imgui/backend/imgui_impl_dx11.h"

#include "tools/ImGuiHelpers.h"
#include "tools/Stopwatch.h"
#include "tools/NameThread.h"

#include <algorithm>

namespace engine
{
	void GameEngine::Init(Game& aGame)
	{
		LOG_SYS_INFO("Initializing Game Engine to run [" + aGame.AsciiName() + "]");
		
		myGame = &aGame;
		
		tools::Stopwatch stopWatch;
		{
			tools::TimedScope scopeTimer(stopWatch);
			AssetManager::GetInstance().Init("data/assets", "data/baked");
#ifdef _DEBUG
			AssetManager::GetInstance().Preload();
#endif // _DEBUG

			GraphicsEngine::GetInstance().Init(SettingsManager::GetInstance().myWindowSize.Get());



			IMGUI_CHECKVERSION();
			ImGui::CreateContext();

			SettingsManager::GetInstance().LoadOrDefaultImGuiStyle();
			ImGui_ImplDX11_Init(GraphicsEngine::GetInstance().GetFrameWork().GetDevice(), GraphicsEngine::GetInstance().GetFrameWork().GetContext());
			ImGui_ImplWin32_Init(WindowManager::GetInstance().GetWindowHandle());
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

		old_betsy_imgui::WindowControl::Window("Engine", [&]() { EngineWindow(); });
		old_betsy_imgui::WindowControl::Window("Performance", PerformanceWindow);
	}

	void GameEngine::EngineWindow()
	{

	}

	void GameEngine::PerformanceWindow()
	{
		static bool accumulate = true;
		ImGui::Checkbox("Accumulate", &accumulate);

		struct namedThread
		{
			std::thread::id myId;
			tools::TimeTree* myRoot;
			std::string myName;
			auto operator <=>(const namedThread& aOther) const { return myId <=> aOther.myId; }
		};
		
		std::vector<namedThread> sortedThreads;

		{
			tools::LockedResource<tools::RootCollection> handle = tools::AllRoots();
			for (std::pair<std::thread::id, tools::TimeTree*> threadTimeRoot : handle.Get())
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
			tools::FlushTimeTree();
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
				GraphicsEngine::GetInstance().BeginFrame(tools::V4f(0.6f + 0.1f * cos(tools::GetTotalTime()), 0.5f  + 0.2f * cos(tools::GetTotalTime() / 1.8f), 0.5f + 0.3f * cos(tools::GetTotalTime() / 3.2f), 1.f));
				GraphicsEngine::GetInstance().RenderFrame();
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
				GraphicsEngine::GetInstance().EndFrame();
			}
		}
	}

	void GameEngine::Update()
	{
		PERFORMANCETAG("Engine Update");
		WindowManager::GetInstance().Update();
	}
}

