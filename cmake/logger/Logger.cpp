#include <mutex>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <filesystem>

#include "common/WindowsIncludeWrapper.h"

#include <WinUser.h>
#include <ShObjIdl.h>

#include "tools/TimeHelper.h"

#include "logger/Logger.h"

#include "imgui/WindowControl.h"


namespace logger
{
	LoggerType Filter = Type::All;
	LoggerType Halting = Type::None;

	std::unordered_map<LoggerType, std::string> FileMapping;
	std::unordered_map<LoggerType, char> ColorMapping;
	std::unordered_map<std::string, std::ofstream> OpenFiles;

	float RapportTimeStamp = tools::GetTotalTime();

	struct LoggerNode
	{
		bool myIsOpen = true;
		size_t myCount = 0;
		std::unordered_map<std::string, LoggerNode*> mySubNodes;
	};

	std::unordered_map<LoggerType, LoggerNode> Roots;

	HWND window = NULL;
	ITaskbarList3* pTaskbar = nullptr;
	HICON errorIcon;
	HICON warningIcon;

	const char DefaultConsoleColor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
	const char* GetSeverity(LoggerType aType)
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
			return "Verbose";
		case Type::SystemCrash:
			return "Crash";
		default:
			return "Unkown";
		}
#else
		return "";
#endif
	}
	std::recursive_mutex ourMutex;
	static std::vector<std::pair<LoggerType, std::pair< std::string, char>>> ImguiLog;

	void Rapport(LoggerType aType, const std::string& aFile, const size_t aLine, const std::string& aError, const std::vector<std::string>& aArguments)
	{
#if USELOGGER
		std::string toPrint =  "[" + aFile + ":" + std::to_string(aLine) + "]: " + aError;

		for (size_t i = 0; i < sizeof(LoggerType) * CHAR_BIT; i++)
		{
			if (BIT(i) & aType)
			{
				auto& rootNode = Roots[BIT(i)];
				++rootNode.myCount;

				if (rootNode.mySubNodes.count(aFile) == 0)
				{
					rootNode.mySubNodes[aFile] = new LoggerNode ();
				}

				LoggerNode* currentNode = rootNode.mySubNodes[aFile];
				currentNode->myCount++;

				if (currentNode->mySubNodes.count(aError) == 0)
				{
					currentNode->mySubNodes[aError] = new LoggerNode();
				}
				currentNode = currentNode->mySubNodes[aError];
				currentNode->myCount++;

				for (const std::string& arg : aArguments)
				{
					toPrint += ", " + arg;

					if (currentNode->mySubNodes.count(arg) == 0)
					{
						currentNode->mySubNodes[arg] = new LoggerNode();
					}
					currentNode = currentNode->mySubNodes[arg];
					currentNode->myCount++;
				}
			}
		}

		Log(aType, toPrint);
#if USEIMGUI
		old_betsy_imgui::WindowControl::SetOpenState("Errors & Warnings", true);
		RapportTimeStamp = tools::GetTotalTime();
#endif // USEIMGUI

#endif
	}

	void RapportWindow()
	{
#if USELOGGER && USEIMGUI
		old_betsy_imgui::WindowControl::Window("Errors & Warnings", [&]()
			{
				std::lock_guard lock(ourMutex);
				static auto ConsoleToImVec4 = [](char input)->  ImVec4
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
				std::function<void(LoggerNode* node)> Unpack_internal_help;
				static auto UnpackNode = [&](LoggerNode* node) -> void 
				{
					for (auto& i : node->mySubNodes)
					{
						if (i.second->mySubNodes.empty())
						{
							if (i.second->myCount > 1)
							{
								ImGui::Text(PFSTRING " (" PFSIZET ")", i.first.c_str(), i.second->myCount);
							}
							else
							{
								ImGui::Text(PFSTRING, i.first.c_str());
							}
						}
						else
						{
							const char* fmt = i.second->myIsOpen ? PFSTRING : PFSTRING " (" PFSIZET ")";

							if (ImGui::TreeNode(i.first.c_str(), fmt, i.first.c_str(), i.second->myCount))
							{
								Unpack_internal_help(i.second);
								i.second->myIsOpen = true;
								ImGui::TreePop();
							}
							else
							{
								i.second->myIsOpen = false;
							}
						}
					}
				};
				Unpack_internal_help = UnpackNode;


				static std::unordered_map<LoggerType, std::string> nameLookup
				{
					{SystemInfo,	"SystemInfo"},
					{SystemError,	"SystemError"},
					{SystemCrash,	"SystemCrash"},
					{SystemWarning,	"SystemWarning"},
					{SystemVerbose,	"SystemVerbose"},
					{SystemNetwork,	"SystemNetwork"},
					{Info,			"Info"},
					{Warning,		"Warning"},
					{Error,			"Error"},
					{Verbose,		"Verbose"}
				};


				float now = tools::GetTotalTime();
				if (ImGui::IsWindowHovered())
				{
					RapportTimeStamp = now;
				}

				for (auto& root : Roots)
				{
					ImVec4 col = ConsoleToImVec4(ColorMapping[root.first]);
					col.w = 1.f - CLAMP(0, 1, (now - RapportTimeStamp) * 0.3f);

					ImGui::PushStyleColor(ImGuiCol_Text, col);

					ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
					if (ImGui::TreeNode(nameLookup[root.first].c_str()))
					{
						UnpackNode(&root.second);
						ImGui::TreePop();
					}
					ImGui::PopStyleColor(1);
				}
			}, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus, []()
			{
				ImGui::SetNextWindowPos(ImVec2(0, 0));
				ImGui::SetNextWindowSize(ImVec2(500, 400));
			});
#endif // USELOGGER
	}

	void Log(LoggerType aType, const std::string& aMessage)
	{
#if USELOGGER
		std::lock_guard lock(ourMutex);
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
			LOG_SYS_ERROR("Logging using multiple filters is not supported");
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
			MessageBoxA(NULL, aMessage.c_str(), "A halting error occurred", MB_OK);
		}
#endif
	}

	void SetFilter(LoggerType aFilter)
	{
#if USELOGGER
		if ((aFilter & Halting) != Halting)
		{
			LOG_SYS_ERROR("Trying to set logger filter which does not encompass all halting types");
			return;
		}
		Filter = aFilter;
#endif
	}

	void SetHalting(LoggerType aFilter)
	{
#if USELOGGER
		if ((aFilter & Filter) != aFilter)
		{
			LOG_SYS_ERROR("Trying to set logger halting which is not encompassed completely by filter");
			return;
		}
		Halting = aFilter;
#endif
	}
	void Map(LoggerType aMessageType, std::string aOutputFile)
	{
#if USELOGGER
		aOutputFile = "logs/" + aOutputFile + ".log";
		for (LoggerType i = 0; i < sizeof(LoggerType) * CHAR_BIT; i++)
		{
			LoggerType bit = aMessageType & (1ULL << i);
			if (bit != 0)
			{
				LoggerType messageType = static_cast<Type>(bit);

				if (FileMapping.count(messageType) != 0)
				{
					LOG_SYS_ERROR("Type is already mapped: ", std::to_string(messageType));
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
						LOG_SYS_ERROR("Could not open file for logging", aOutputFile);
					}
				}
			}
		}
#endif
	}
	void UnMap(LoggerType aMessageType)
	{
#if USELOGGER
		for (LoggerType i = 0; i < sizeof(LoggerType) * CHAR_BIT; i++)
		{
			LoggerType bit = aMessageType & (1ULL << i);
			if (bit != 0)
			{
				LoggerType messageType = static_cast<Type>(bit);

				if (FileMapping.count(messageType) == 0)
				{
					LOG_SYS_ERROR("Trying to unmap type which is not mapped: ", std::to_string(messageType));
					continue;
				}
				LOG_SYS_INFO("Unmapping: " + std::to_string(messageType) + " from: " + FileMapping[messageType]);

				std::string file = FileMapping[messageType];
				FileMapping.erase(messageType);
				for (auto& fileMap : FileMapping)
				{
					if (fileMap.second == file)
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
	void SetColor(LoggerType aMessageType, char aColor)
	{
#if USELOGGER
		for (LoggerType i = 0; i < sizeof(LoggerType) * CHAR_BIT; i++)
		{
			LoggerType bit = aMessageType & (1ULL << i);
			if (bit != 0)
			{
				LoggerType messageType = static_cast<Type>(bit);

				ColorMapping[messageType] = aColor;
			}
		}
#endif
	}
	void SetupIcons(void* aHWND)
	{
#if USELOGGER
		window = reinterpret_cast<HWND>(aHWND);
		HRESULT hr = CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_ITaskbarList3, reinterpret_cast<void**>(&pTaskbar));
		if (FAILED(hr))
		{
			std::cerr << "Failed to set up logger icons" << std::endl;
			return;
		}

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
		std::unique_lock lock(ourMutex);
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
				delta = static_cast<char>(ImGui::GetIO().MouseWheel * -2.f);
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
				at = static_cast<size_t>(ImGui::GetScrollY() / itemsize);
			}
			ImGui::EndChild();
			ImGui::NextColumn();
			ImGui::BeginChild("buttons", ImVec2(150, itemsize * rows));
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
			//static_assert(sizeof(names) / sizeof(names[0]) > sizeof(LoggerType)* CHAR_BIT, "Not all loggertypes have names");

			for (size_t i = 0; i < sizeof(LoggerType) * CHAR_BIT; i++)
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

		}
		ImGui::EndChild();

#endif // USEIMGUI
	}
}


