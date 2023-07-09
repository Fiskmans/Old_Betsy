#ifndef ENGINE_IMGUI_MANAGER_H
#define ENGINE_IMGUI_MANAGER_H

#include "engine/ImGuiWindow.h"

#include "tools/Singleton.h"

#include <vector>

namespace engine
{
	class ImGuiManager : public fisk::tools::Singleton<ImGuiManager>
	{
	public:
		void Init();
		void Update();

		void AddWindow(ImGuiWindow* aWindow);
		void RemoveWindow(ImGuiWindow* aWindow);

	private:

		std::vector<ImGuiWindow*> myWindows;
	};
}

#endif