#include <unordered_map>
#include <string>
#include <functional>

#include "imgui/imgui.h"

typedef int ImGuiWindowFlags;

namespace old_betsy_imgui
{
	class WindowControl
	{
		static std::unordered_map<std::string, bool> myOpenWindows;
	public:
		static void DrawWindowControl();
		static bool Window(const char* aLabel, std::function<void()> aContent, ImGuiWindowFlags flags = (ImGuiWindowFlags)0, std::function<void()> aWindowSettings = std::function<void()>());
		static void SetOpenState(const char* aLabel, bool aState);
		static bool GetWindowState(const char* aLabel);
	};
}