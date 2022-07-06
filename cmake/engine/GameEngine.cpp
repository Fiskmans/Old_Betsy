
#include "engine/GameEngine.h"
#include "engine/SettingsManager.h"
#include "engine/graphics/GraphicEngine.h"
#include "engine/graphics/WindowManager.h"

#include "engine/assets/AssetManager.h"

#include "imgui/imgui.h"
#include "imgui/WindowControl.h"
#include "imgui/backend/imgui_impl_win32.h"
#include "imgui/backend/imgui_impl_dx11.h"


#include "tools/Stopwatch.h"

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
		old_betsy_imgui::WindowControl::DrawWindowControl();
		if (ImGui::Begin("Hello"))
		{
			static size_t frameCount = 0;
			ImGui::Text("hello " PFSIZET, frameCount++);

			ImVec2 pos = ImGui::GetCursorPos();

			ImGui::Text("window pos x:" PFFLOAT " y:" PFFLOAT, pos.x, pos.y);
		}
		ImGui::End();
	}

	void GameEngine::Run()
	{
		while (!myGame->WantsExit())
		{
			PERFORMANCETAG("main_update_loop");

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
		PERFORMANCETAG("engine_update");
		WindowManager::GetInstance().Update();
	}
}

