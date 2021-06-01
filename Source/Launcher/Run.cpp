#include <pch.h>
#include <Game.h>
#include "WindowHandler.h"
#include <DirectX11Framework.h>
#include "Button.h"
#include <shellapi.h>

#if USEIMGUI
#include <SvnIntegration.h>
#include <future>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <WindowControl.h>
#endif // !_RETAIL

#include "AudioManager.h"


#include <GraphicEngine.h>
#include <MemoryChunk.h>
#define TOTALMEMORYSIZE (1ULL<<28)



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
#endif // !_RETAIL

int Run()
{

#ifdef _DEBUG
	const wchar_t* commLine = GetCommandLineW();
	int argc;
	LPWSTR* argv = CommandLineToArgvW(commLine, &argc);
	for (size_t i = 0; i < argc; i++)
	{
		DebugTools::CommandLineFlags.emplace(argv[i]);
	}
#endif //_DEBUG
	{
#if BOOTUPDIAGNOSTIC
		long long startTime = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch().count();
#endif // !_RETAIL

#if USELOGGER
		AllocConsole();
#endif

#if USEIMGUI

		char pendingFiles[2048];
		*pendingFiles = '\0';
		bool pendingSvn = false;
		std::future<bool> pendingSvnResult;// = std::async(SvnIntegration::CheckForUpdates, pendingFiles, 2048);

		char svnLogg[8192];
		svnLogg[0] = '\0';
		//std::async(SvnIntegration::LogMessages,svnLogg,8192);

#endif // !_RETAIL

#if USEMEMORYCONTROL
	//Setup memory manegement
		MemoryChunk* chunk = new MemoryChunk(TOTALMEMORYSIZE);
		Allocator* allocator = new NewFromMemoryBlock(chunk);
		Memory::SetGlobalAllocator(allocator);
#endif // USEMEMORYCONTROL


		{
#if USELOGGER
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
#endif
			static float mainVolume = 0.5f;
#ifdef _DEBUG
			mainVolume = 0.f;
#endif

			Window::WindowData windowData;
			windowData.myX = 0;
			windowData.myY = 0;
			windowData.myWidth = 1920;
			windowData.myHeight = 1080;

			CGraphicsEngine engine;

			PostMaster::Create();

			if (!engine.Init(windowData, nullptr, nullptr))
			{
				SYSCRASH("Could not start engine");
				return -1;
			}

			Button::SetSpriteFactory(&engine.GetSpriteFactory());

			Game* game = new Game();

			AudioManager audioManager;
			audioManager.Init();
			bool shouldRun = game->Init(engine.GetWindowHandler(), &engine.GetWindowHandler()->GetInputHandler(), engine.GetLightLoader(), &engine.GetSpriteFactory(), engine.GetFrameWork(), &audioManager, engine.GetSpriteRenderer());

			engine.SubscribeToMessages();


#if USEIMGUI

			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			//ImGuiIO& io = ImGui::GetIO(); enable keyboard and gamepad input using this

			LoadOrDefaultImGuiStyle();
			ImGui_ImplDX11_Init(engine.GetFrameWork()->GetDevice(), engine.GetFrameWork()->GetContext());
			ImGui_ImplWin32_Init(engine.GetWindowHandler()->GetWindowHandle());
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

			int framesWithMessage = 0;
			while (shouldRun)
			{
				{
					PERFORMANCETAG("Main loop");
#if USEIMGUI
					if (pendingSvnResult.valid() && pendingSvnResult.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
					{
						pendingSvn = pendingSvnResult.get();
					}
#endif // !_RETAIL

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

					if (pendingSvn)
					{
						ImGui::SetNextWindowPos(ImVec2(1100, 20), ImGuiCond_Appearing);
						if (ImGui::Begin("New update available! (V)owo(V)", &pendingSvn, ImGuiWindowFlags_NoResize))
						{
							if (ImGui::Button("Update"))
							{
								SvnIntegration::Update();
								pendingSvn = false;
							}
							ImGui::Text(pendingFiles);
						}
						ImGui::End();
					}
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
								PostMaster::GetInstance()->SendMessages(MessageType::SerializePhysXObjects);
							}

#if BOOTUPDIAGNOSTIC
							ImGui::Text("Startup time %I64d milliseconds", initTimeDelta);

#endif
#if USEIMGUI
							engine.Imgui();
#endif
							if (ImGui::DragFloat("Volume", &mainVolume, 0.01f, 0.0f, 1.f))
							{
#if USEAUDIO
								audioManager.SetMasterVolume(mainVolume);
#endif
							}
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

#if USEMEMORYCONTROLS
							static long long MemAvailability = chunk->GetAvailability();
							long long newAvailability = chunk->GetAvailability();
							long long memoryDelta = newAvailability - MemAvailability;

							static float MemTracker[trackerSize] = { static_cast<float>(TOTALMEMORYSIZE - newAvailability) / static_cast<float>(TOTALMEMORYSIZE) };
							MemTracker[trackerSize - 1] = static_cast<float>(TOTALMEMORYSIZE - newAvailability) / static_cast<float>(TOTALMEMORYSIZE);
							memmove(MemTracker, MemTracker + 1, sizeof(float) * (trackerSize - 1));
							ImGui::PlotLines("", MemTracker, trackerSize, 0, nullptr, 0.f, 1.f);
							ImGui::SameLine();
							ImGui::TextColored(ImVec4(1.f, 1.0f - MemTracker[trackerSize - 2], 1.0f - MemTracker[trackerSize - 2], 1), "Memory %.2f%s", MemTracker[trackerSize - 2] * 100.f, "%");

							ImGui::TextColored(ImVec4(static_cast<float>(memoryDelta <= 0), static_cast<float>(memoryDelta >= 0), static_cast<float>(memoryDelta == 0), 1), "Available Memory: %dmb", static_cast<int>(newAvailability / (1ULL << 20)));
							ImGui::Text("Memory Fragmentation: %d chunks", chunk->GetFragmentation());
							if (ImGui::Button("Defragment memory"))
							{
								chunk->DeFragment();
							}
							MemAvailability = newAvailability;
#endif
							if (ImGui::TreeNode("Svn"))
							{
								if (ImGui::Button("Check for updates"))
								{
									if (!SvnIntegration::IsSupported())
									{
										ImGui::OpenPopup("SvnNotSupportedPopup");
									}
									pendingSvnResult = std::async(SvnIntegration::CheckForUpdates, pendingFiles, sizeof(pendingFiles) / sizeof(pendingFiles[0]));
								}

								if (ImGui::TreeNode("Log messages"))
								{
									ImGui::Text(svnLogg);
									ImGui::TreePop();
								}

								ImGui::TreePop();
							}

							if (ImGui::TreeNode("Info"))
							{
								ImGui::Text("Size of pointLightuffer: " STRINGVALUE(NUMBEROFPOINTLIGHTS));

								ImGui::TreePop();
							}
							if (ImGui::BeginPopupModal("SvnNotSupportedPopup"))
							{
								ImGui::Text("Svn integration is not supported on this machine (yet). Install svn command line tools to enable.");
								if (ImGui::Button("Help"))
								{
									ImGui::OpenPopup("SvnInstallHelp");
								}
								ImGui::SameLine();
								if (ImGui::Button("Close"))
								{
									ImGui::CloseCurrentPopup();
								}

								if (ImGui::BeginPopupModal("SvnInstallHelp"))
								{
									ImGui::BulletText("Navigate to '/SharedInstallers' in the project folder");
									ImGui::BulletText("Run the 'TortoiseSVN...' and go through the installer");
									ImGui::BulletText("When you get to the custom setup step, make sure that\n'command line client tools' is selected to install");
									ImGui::BulletText("Reboot");
									if (ImGui::Button("Close"))
									{
										ImGui::CloseCurrentPopup();
									}

									ImGui::EndPopup();
								}
								ImGui::EndPopup();
							}
						});

#endif // !USEIMGUI


					static long long lastTime = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now()).time_since_epoch().count();
					long long currentTime = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now()).time_since_epoch().count();
					long long delta = currentTime - lastTime;
					lastTime = currentTime;
					{
						PERFORMANCETAG("Engine run");
						engine.BeginFrame(clearColor);
						shouldRun &= game->Run(&engine, CAST(float, double(delta) / (1000.0 * 1000.0)));
					}

#if USEAUDIO
					audioManager.Update(CAST(float, double(delta) / (1000.0 * 1000.0)));
#endif
				}
#if USEIMGUI && TRACKPERFORMANCE
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
				{
					PERFORMANCETAG("Main loop");
#if USEIMGUI
					{
						PERFORMANCETAG("Imgui Drawing [old]");
						ImGui::Render();
						ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
					}
#endif // !USEIMGUI
					{
						PERFORMANCETAG("End frame [old]");
						engine.EndFrame();
					}
				}


				if (framesWithMessage < 5)
				{
					engine.GetFrameWork()->Resize(engine.GetWindowHandler()->GetWindowHandle());
					framesWithMessage++;
				}

			}

#if USEIMGUI
			ImGui_ImplDX11_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();
#endif // !USEIMGUI


			engine.UnsubscribeToMessages();
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