#include <pch.h>
#include <Game.h>
#include "WindowHandler.h"
#include <DirectX11Framework.h>
#include "Button.h"
#include <shellapi.h>

#if USEIMGUI
#include "Tools\imgui\backend\imgui_impl_dx11.h"
#include "Tools\imgui\backend\imgui_impl_win32.h"
#include "RenderScene.h"
#endif // !_RETAIL

#include "AudioManager.h"
#include <GraphicEngine.h>



#if USEIMGUI
void SaveImGuiStyle()
{
	static_assert(sizeof(char) == 1, "Double check");
	std::ofstream stream;
	stream.open("ImGuiStyle.ini", std::ios::binary | std::ios::out);
	stream.write(reinterpret_cast<char*>(&ImGui::GetStyle()), sizeof(*(&ImGui::GetStyle())));
}

void LoadOrDefaultImGuiStyle()
{
	static_assert(sizeof(char) == 1, "Double check");
	std::ifstream stream;
	stream.open("ImGuiStyle.ini", std::ios::binary | std::ios::in);
	if (stream)
	{
		stream.read(reinterpret_cast<char*>(&ImGui::GetStyle()), sizeof(*(&ImGui::GetStyle())));
	}
	else
	{
		ImGui::StyleColorsDark();
	}
}
#endif // !USEIMGUI

int Run()
{

	{
#if BOOTUPDIAGNOSTIC
		long long startTime = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch().count();
#endif // !_RETAIL

#if USELOGGER
		AllocConsole();
#endif
		FILE* fp;
		freopen_s(&fp, "CONOUT$", "w", stdout);
		freopen_s(&fp, "CONIN$", "r", stdin);
		freopen_s(&fp, "CONOUT$", "w", stderr);

		Logger::SetFilter(Logger::Type::AnyWarning | Logger::Type::AllSystem & ~Logger::Type::AnyVerbose);
		Logger::SetHalting(Logger::Type::SystemCrash);


		Logger::Map(Logger::Type::AnyGame, "info");
		Logger::Map(Logger::Type::AnySystem, "system");
		Logger::SetColor(Logger::Type::AnyError, FOREGROUND_RED | FOREGROUND_INTENSITY);
		Logger::SetColor(Logger::Type::AnyWarning, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		Logger::SetColor(Logger::Type::AnyInfo, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		Logger::SetColor(Logger::Type::SystemNetwork, FOREGROUND_GREEN | FOREGROUND_BLUE);


		{
			static float mainVolume = 0.5f;
#ifdef _DEBUG
			mainVolume = 0.f;
#endif


			CGraphicsEngine engine;

			if (!engine.Init())
			{
				SYSCRASH("Could not start engine");
				return -1;
			}

			Game* game = new Game();

			AudioManager audioManager;
			audioManager.Init();
			bool shouldRun = game->Init(&WindowHandler::GetInstance().GetInputHandler(), engine.GetLightLoader(), &engine.GetSpriteFactory(), engine.GetFrameWork(), &audioManager, engine.GetSpriteRenderer());

#if USEIMGUI
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();

			LoadOrDefaultImGuiStyle();
			ImGui_ImplDX11_Init(engine.GetFrameWork()->GetDevice(), engine.GetFrameWork()->GetContext());
			ImGui_ImplWin32_Init(WindowHandler::GetInstance().GetWindowHandle());
#endif // !USEIMGUI


			MSG windowMessage;
			WIPE(windowMessage);


			float clearColor[4] = { 0.8f,0.36f,0.7f,1.f };


#if BOOTUPDIAGNOSTIC
			long long initTime = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch().count();
			long long initTimeDelta = initTime - startTime;
			if (initTimeDelta > 10000)
			{
				SYSERROR("Game took more than 10 seconds to start");
			}
			else if (initTimeDelta > 5000)
			{
				SYSWARNING("Game took more than 5 seconds to start");
			}
#endif // BOOTUPDIAGNOSTIC

			while (shouldRun)
			{
				{
					PERFORMANCETAG("Main loop");

#ifdef _RETAIL
					while (ShowCursor(FALSE) >= 0)
					{
					}
#endif
					{
						PERFORMANCETAG("Winmessage Parsing");
						while (PeekMessage(&windowMessage, nullptr, 0, 0, PM_REMOVE))
						{
							TranslateMessage(&windowMessage);
							DispatchMessage(&windowMessage);

							if (windowMessage.message == WM_QUIT || windowMessage.message == WM_DESTROY)
							{
								shouldRun = false;
							}
						}
					}

#if USEIMGUI
					ImGui_ImplDX11_NewFrame();
					ImGui_ImplWin32_NewFrame();
					ImGui::NewFrame();

					WindowControl::DrawWindowControl();
#if TRACKPERFORMANCE
					WindowControl::Window("Diagnostic", []()
					{
						Tools::TimeTree* at = Tools::GetTimeTreeRoot();
						static bool accumulative = true;
						ImGui::Checkbox("Accumulative", &accumulative);
						Tools::DrawTimeTree(at);
						if (!accumulative)
						{
							Tools::FlushTimeTree();
						}
					});
#endif

					RenderScene::GetInstance().Imgui();
#if USELOGGER
					WindowControl::Window("Console log", []()
						{
							Logger::ImGuiLog();
						});
#endif
					DirectX11Framework::Imgui();
					WindowControl::Window("Engine", [&]()
						{
							if (ImGui::Button("Crash"))
							{
								*reinterpret_cast<int*>(0) = 10;
							}
							if (ImGui::Button("SerilizePhysXObjects"))
							{
								PostMaster::GetInstance().SendMessages(MessageType::SerializePhysXObjects);
							}

#if BOOTUPDIAGNOSTIC
							ImGui::Text("Startup time %I64d milliseconds", initTimeDelta);

#endif
#if USEIMGUI
							engine.Imgui();
#endif
#if USEAUDIO
							if (ImGui::DragFloat("Volume", &mainVolume, 0.01f, 0.0f, 1.f))
							{
								audioManager.SetMasterVolume(mainVolume);
							}
#endif
							if (ImGui::Button("Edit Window Style"))
							{
								ImGui::OpenPopup("ImguiStyleEditor");
							}
							if (ImGui::BeginPopup("ImguiStyleEditor"))
							{
								if (ImGui::Button("Save Changes"))
								{
									SaveImGuiStyle();
								}
								ImGui::SameLine();
								if (ImGui::Button("Reload"))
								{
									LoadOrDefaultImGuiStyle();
								}
								ImGui::ShowStyleEditor();
								ImGui::EndPopup();
							}
							ImGui::SameLine();
							static bool showDemo = false;
							ImGui::Checkbox("Show demo window", &showDemo);
							if (showDemo)
							{
								ImGui::ShowDemoWindow();
							}
							ImGui::ColorEdit3("BackgroundColor", clearColor);

							const size_t trackerSize = 1 << 8;
							static long long lastTime = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now()).time_since_epoch().count();
							long long currentTime = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now()).time_since_epoch().count();
							long long delta = currentTime - lastTime;
							lastTime = currentTime;
							float fps = 1.f / (float(delta) / 1000.f / 1000.f);
							static float fpsTracker[trackerSize] = { fps };
							fpsTracker[trackerSize - 1] = fps;
							memmove(fpsTracker, fpsTracker + 1, sizeof(float) * (trackerSize - 1));
							ImGui::PlotLines(("FPS " + (std::to_string(fps).substr(0, 4))).c_str(), fpsTracker, trackerSize, 0, nullptr, 0, 60);
							ImGui::Text("Time per frame: %.1f milliseconds", float(delta) / 1000.f);


							if (ImGui::TreeNode("Info"))
							{
								ImGui::Text("Size of pointLightuffer: " STRINGVALUE(NUMBEROFPOINTLIGHTS));

								ImGui::TreePop();
							}
						});

#endif // !USEIMGUI


				}
			}

#if USEIMGUI
			ImGui_ImplDX11_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();
#endif // !USEIMGUI

			delete game;

#if USEAUDIO
			audioManager.ShutDown();
#endif
		}

#if USEMEMORYCONTROLS
		Memory::DefaultGlobalAllocator();

		delete allocator;
		delete chunk;
#endif

	}

	return 0;
}