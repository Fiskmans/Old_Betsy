#include <pch.h>
#include "Logger.h"
#include <iostream>
#include <map>
#include <fstream>
#include <chrono>
#include <ctime>
#include <time.h>
#include <Windows.h>
#include <WinUser.h>
#include <ShObjIdl.h>
#if USEIMGUI
#include <imgui.h>
#include <WindowControl.h>
#endif
#ifdef _DEBUG
#include "../GamlaBettan/DebugTools.h"
#endif // _DEBUG
#if USELOGGER
#include <bitset> 
#endif

#include <mutex>

namespace Logger
{
	LOGGERTYPE Filter = Type::All;
	LOGGERTYPE Halting = Type::None;

	std::map<LOGGERTYPE, std::string> FileMapping;
	std::map<LOGGERTYPE, char> ColorMapping;
	std::map<std::string, std::ofstream> OpenFiles;

	std::array<std::pair<std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::pair<std::string, int>>>>, char>, sizeof(LOGGERTYPE) * CHAR_BIT> Rapports;
	float RapportTimeStamp = Tools::GetTotalTime();


	HWND window = nullptr;
	ITaskbarList3* pTaskbar = nullptr;
	HICON errorIcon;
	HICON warningIcon;

	const char DefaultConsoleColor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
	const char* GetSeverity(LOGGERTYPE aType)
	{
#if USELOGGER
		switch (aType)
		{
		case Type::SystemInfo:
		case Type::Info:
			return "Info";
		case Type::SystemError:
		case Type::Error:
			return "Error";
		case Type::SystemWarning:
		case Type::Warning:
			return "Warning";
		case Type::SystemVerbose:
		case Type::Verbose:
			return "Vebose";
		case Type::SystemCrash:
			return "Crash";
		default:
			return "Unkown";
		}
#else
		return "";
#endif
	}
	std::recursive_mutex outMutex;
	static std::vector<std::pair<LOGGERTYPE, std::pair< std::string, char>>> ImguiLog;

	void Rapport(LOGGERTYPE aType, const std::string& aCategory, const std::string& aError, const std::string& aArgument)
	{
#if USELOGGER
		for (size_t i = 0; i < sizeof(LOGGERTYPE) * CHAR_BIT; i++)
		{
			if (BIT(i) & aType)
			{
				auto& list = Rapports[i].first[aCategory][aError];
				bool exist = false;
				for (auto& i : list)
				{
					if (i.first == aArgument)
					{
						++i.second;
						exist = true;
						break;
					}
				}
				if (!exist)
				{
					list.push_back(std::make_pair(aArgument, 1));
				}
			}
		}
#if USEIMGUI
		//WindowControl::SetOpenState("Errors & Warnings", true);
		RapportTimeStamp = Tools::GetTotalTime();
#endif // USEIMGUI

#endif
	}

	void RapportWindow()
	{
#if USELOGGER && USEIMGUI
		WindowControl::Window("Errors & Warnings", []()
			{
				std::lock_guard lock(outMutex);
				static const auto ConsoleToImVec4 = [](char input)-> ImVec4
				{
					float intens = input & FOREGROUND_INTENSITY ? 1 : 0.5f;
					return ImVec4
					(
						input & FOREGROUND_RED ? intens : 0,
						input & FOREGROUND_GREEN ? intens : 0,
						input & FOREGROUND_BLUE ? intens : 0,
						1
					);
				};
				for (size_t i = 0; i < sizeof(LOGGERTYPE) * CHAR_BIT; i++)
				{
					if (!Rapports[i].first.empty())
					{
						ImVec4 col = ConsoleToImVec4(Rapports[i].second);
						float now = Tools::GetTotalTime();
						col.w = 1.f - CLAMP(0, 1, (now - RapportTimeStamp) * 0.3f);

						ImGui::PushStyleColor(ImGuiCol_Text, col);
						const char* names[] =
						{
							"SystemInfo",
							"SystemError",
							"SystemCrash",
							"SystemWarning",
							"SystemVerbose",
							"SystemNetwork",
							//game
							"Info",
							"Warning",
							"Error",
							"Verbose"
						};

						if (ImGui::IsWindowHovered())
						{
							RapportTimeStamp = now;
						}
						ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
						if (ImGui::TreeNode(names[i]))
						{
							for (auto& file : Rapports[i].first) //files
							{
								ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
								if (ImGui::TreeNode(file.first.c_str()))
								{
									for (auto& error : file.second) // errors
									{
										ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
										if (ImGui::TreeNode(error.first.c_str()))
										{
											for (auto& arg : error.second)
											{
												if (arg.second > 1)
												{
													ImGui::BulletText((arg.first + " (%d)").c_str(), arg.second);
												}
												else
												{
													ImGui::BulletText(arg.first.c_str());
												}
											}
											ImGui::TreePop();
										}
									}
									ImGui::TreePop();
								}
							}
							ImGui::TreePop();
						}

						ImGui::PopStyleColor(1);
					}
				}
			}, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus, []()
			{
				ImGui::SetNextWindowPos(ImVec2(0, 0));
			});
#endif // USELOGGER
	}

	void Log(LOGGERTYPE aType, const std::string& aMessage)
	{
#if USELOGGER
		std::lock_guard lock(outMutex);
		static HANDLE consoleHandle;
		static bool first = true;
		if (first)
		{
			consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
			first = true;
		}

		if (pTaskbar)
		{
			if (aType & Type::AnyError)
			{
				pTaskbar->SetOverlayIcon(window, errorIcon, L"Error");
			}

			if (aType & Type::AnyWarning)
			{
				pTaskbar->SetOverlayIcon(window, warningIcon, L"Warning");
			}
		}
		char color = DefaultConsoleColor;
		bool reset = false;
		if ((aType != 0) && ((aType & (aType - 1)) == 0)) // is power of 2
		{
			if (FileMapping.count(aType) != 0)
			{
				char buffer[256];
				memset(buffer, '\0', 256);
				std::time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
				ctime_s(buffer, 256, &time);
				if (*buffer != '\0')
				{
					buffer[strlen(buffer) - 1] = '\0';
				}
				OpenFiles[FileMapping[aType]] << "[" << buffer << "] [" << GetSeverity(aType) << "] " << aMessage << std::endl;
			}

			if (ColorMapping.count(aType) != 0)
			{
				color = ColorMapping[aType];
				SetConsoleTextAttribute(consoleHandle, color);
				reset = true;
			}

			ImguiLog.push_back(std::make_pair(aType, std::make_pair(aMessage, color)));
		}
		else
		{
			SYSERROR("Logging using multiple filters is not supported", "");
		}
		if ((aType & Filter) != 0)
		{
			std::cout << aMessage << std::endl;
		}

		if (reset)
		{
			SetConsoleTextAttribute(consoleHandle, DefaultConsoleColor);
		}

		if ((aType & Halting) != 0)
		{
			SetActiveWindow(GetConsoleWindow());
			MessageBoxA(NULL, aMessage.c_str(), "A halting error occured", MB_OK);
		}
#endif
	}

	void Log(LOGGERTYPE aType, const std::wstring& aMessage)
	{
#if USELOGGER
		std::cout << "wstring are not supported" << std::endl;
#endif
	}
	void SetFilter(LOGGERTYPE aFilter)
	{
#if USELOGGER
		if ((aFilter & Halting) != Halting)
		{
			SYSERROR("Trying to set logger filter which does not encompass all halting types", "");
			return;
		}
		Filter = aFilter;
#endif
	}

	void SetHalting(LOGGERTYPE aFilter)
	{
#if USELOGGER
		if ((aFilter & Filter) != aFilter)
		{
			SYSERROR("Trying to set logger halting which is not encompassed completely by filter", "");
			return;
		}
		Halting = aFilter;
#endif
	}
	void Map(LOGGERTYPE aMessageType, std::string aOutputFile)
	{
#if USELOGGER
		aOutputFile = "logs/" + aOutputFile + ".log";
		for (LOGGERTYPE i = 0; i < sizeof(LOGGERTYPE) * CHAR_BIT; i++)
		{
			LOGGERTYPE bit = aMessageType & (1ULL << i);
			if (bit != 0)
			{
				LOGGERTYPE messageType = static_cast<Type>(bit);

				if (FileMapping.count(messageType) != 0)
				{
					SYSERROR("Type is already mapped: ", std::to_string(messageType));
					return;
				}

				FileMapping[messageType] = aOutputFile;
				if (OpenFiles.count(aOutputFile) == 0)
				{
					if (!std::filesystem::exists("logs/"))
					{
						std::filesystem::create_directories("logs/");
					}

					OpenFiles[aOutputFile].open(aOutputFile); // default construct and open
					if (!OpenFiles[aOutputFile].good())
					{
						SYSERROR("Could not open file for logging", aOutputFile);
					}
				}
			}
		}
#endif
	}
	void UnMap(LOGGERTYPE aMessageType)
	{
#if USELOGGER
		for (LOGGERTYPE i = 0; i < sizeof(LOGGERTYPE) * CHAR_BIT; i++)
		{
			LOGGERTYPE bit = aMessageType & (1ULL << i);
			if (bit != 0)
			{
				LOGGERTYPE messageType = static_cast<Type>(bit);

				if (FileMapping.count(messageType) == 0)
				{
					SYSERROR("Trying to unmap type which is not mapped: ", std::to_string(messageType));
					continue;
				}
				SYSINFO("Unmapping: " + std::to_string(messageType) + " from: " + FileMapping[messageType]);

				std::string file = FileMapping[messageType];
				FileMapping.erase(messageType);
				for (auto& i : FileMapping)
				{
					if (i.second == file)
					{
						return;
					}
				}
				OpenFiles[file].close();
				OpenFiles.erase(file); //destruct
			}
		}
#endif
	}
	void SetColor(LOGGERTYPE aMessageType, char aColor)
	{
#if USELOGGER
		for (LOGGERTYPE i = 0; i < sizeof(LOGGERTYPE) * CHAR_BIT; i++)
		{
			LOGGERTYPE bit = aMessageType & (1ULL << i);
			if (bit != 0)
			{
				LOGGERTYPE messageType = static_cast<Type>(bit);

				ColorMapping[messageType] = aColor;
#if USEIMGUI
				Rapports[i].second = aColor;
#endif // USEIMGUI

			}
		}
#endif
	}
	void SetupIcons(void* aHWND)
	{
#if USELOGGER
		window = reinterpret_cast<HWND>(aHWND);
		HRESULT hr = CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_ITaskbarList3, reinterpret_cast<void**>(&pTaskbar));
		errorIcon = static_cast<HICON>(::LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(129), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR));
		warningIcon = static_cast<HICON>(::LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(130), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR));
#endif
	}
	void Shutdown()
	{
#if USELOGGER
		pTaskbar->Release();
		CoUninitialize();
#endif
	}
	void ImGuiLog()
	{
#if USEIMGUI
		std::unique_lock lock(outMutex);
		auto& buffer = ImguiLog;
		if (ImGui::BeginChild("ConsoleLog", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar))
		{
			static bool autoScroll = true;
			ImGui::Checkbox("Autoscroll to bottom on new ", &autoScroll);
			static const auto ConsoleToImVec4 = [](char input)-> ImVec4
			{
				float intens = input & FOREGROUND_INTENSITY ? 1 : 0.5f;
				return ImVec4
				(
					input & FOREGROUND_RED ? intens : 0,
					input & FOREGROUND_GREEN ? intens : 0,
					input & FOREGROUND_BLUE ? intens : 0,
					1
				);
			};

			static size_t at = 0;


			size_t amount = 0;
			for (auto& i : buffer)
			{
				if (i.first & Filter)
				{
					++amount;
				}
			}
			const static size_t rows = 15;
			char delta = 0;
			if (ImGui::IsWindowHovered())
			{
				delta = ImGui::GetIO().MouseWheel * -2.f;
			}
			bool shouldSetScrollBar = delta != 0;
			if (int(at) + delta < 0)
			{
				at = 0;
			}
			else if (delta + at + rows > amount)
			{
				at = amount - rows;
			}
			else
			{
				at += delta;
			}
			float itemsize = 0;
			ImGui::Columns(3);
			ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() - 180);
			ImGui::SetColumnWidth(1, 30);
			ImGui::SetColumnWidth(2, 150);
			for (size_t i = at, count = 0; count < rows && i < amount; i++)
			{
				if (buffer[i].first & Filter)
				{
					count++;
					float start = ImGui::GetCursorPos().y;
					ImGui::TextColored(ConsoleToImVec4(buffer[i].second.second), "%s", buffer[i].second.first.c_str());
					itemsize = ImGui::GetCursorPos().y - start;
				}
			}
			lock.unlock();

			static size_t last = amount;
			if (autoScroll && last != amount)
			{
				if (amount > rows)
				{
					at = amount - rows;
					shouldSetScrollBar = true;
				}
			}
			last = amount;
			ImGui::NextColumn();
			ImGui::BeginChild("", ImVec2(15, itemsize * rows), false, ImGuiWindowFlags_NoTitleBar);
			ImGui::BeginChild("2", ImVec2(1, itemsize * amount), false, ImGuiWindowFlags_NoScrollbar);
			ImGui::EndChild();
			if (shouldSetScrollBar)
			{
				ImGui::SetScrollY(at * itemsize);
			}
			else
			{
				at = ImGui::GetScrollY() / itemsize;
			}
			ImGui::EndChild();
			ImGui::NextColumn();
			ImGui::BeginChild("buttons",ImVec2(150, itemsize * rows));
			const char* names[] =
			{
				"Sysinfo",
				"Syserror",
				"SysCrash",
				"SysWarning",
				"SysVerbose",
				"SysNetwork",
				"Info",
				"Warning",
				"Error",
				"Verbose",
				nullptr,
				nullptr,
				nullptr,
				nullptr,
				nullptr,
				nullptr
			};
			//static_assert(sizeof(names) / sizeof(names[0]) > sizeof(LOGGERTYPE)* CHAR_BIT, "Not all loggertypes have names");

			for (size_t i = 0; i < sizeof(LOGGERTYPE) * CHAR_BIT; i++)
			{
				bool buf = !!(BIT(i) & Filter);
				const char* name = names[i];
				if (name)
				{
					if (ImGui::Checkbox(name, &buf))
					{
						if (buf)
						{
							Filter |= BIT(i);
						}
						else
						{
							Filter &= ~BIT(i);
						}
					}
				}
			}
			ImGui::EndChild();
			ImGui::NextColumn();

			ImGui::Separator();
			static char commandLine[256];
			static bool first = true;
			if (first)
			{
				WIPE(commandLine);
				first = false;
			}
			ImGui::InputText("", commandLine, 256);
			if (ImGui::IsKeyPressed(VK_RETURN) && (*commandLine) != '\0')
			{
#ifdef _DEBUG
				LOGINFO("[" + DebugTools::myUsername + "]: " + commandLine);
#else
				LOGINFO(commandLine);
#endif // _DEBUG

				WIPE(commandLine);
			}
		}
		ImGui::EndChild();

#endif // USEIMGUI
	}
}


