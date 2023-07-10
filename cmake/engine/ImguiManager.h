#ifndef ENGINE_IMGUI_MANAGER_H
#define ENGINE_IMGUI_MANAGER_H

#include "engine/ImGuiWindow.h"

#include "tools/Singleton.h"

#include <vector>
#include <unordered_map>

namespace engine
{
	class ImGuiManager : public fisk::tools::Singleton<ImGuiManager>
	{
	public:
		void Init();
		void Update();

		void AddWindow(const std::string& aCategory, ImGuiWindow* aWindow);
		void RemoveWindow(ImGuiWindow* aWindow);

	private:

		std::unordered_map<std::string, std::vector<ImGuiWindow*>> myWindows;
	};
}

#endif