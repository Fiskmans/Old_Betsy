#ifndef ENGINE_IMGUI_WINDOW_H
#define ENGINE_IMGUI_WINDOW_H

#include <string>

namespace engine
{
	class ImGuiWindow
	{
	public:
		ImGuiWindow(const std::string& aCategory = "Windows");
		~ImGuiWindow();

		virtual void OnImgui() = 0;
		virtual const char* ImGuiName() = 0;

		bool myImGuiOpen = false;
	};
}

#endif