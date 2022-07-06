#include "imgui/WindowControl.h"
#include "imgui/imgui.h"

#include "common/WindowsIncludeWrapper.h"
#include "common/Macros.h"

#include "tools/TimeHelper.h"

namespace old_betsy_imgui
{
	std::unordered_map<std::string, bool> WindowControl::myOpenWindows;
	void WindowControl::DrawWindowControl()
	{
		if (ImGui::GetIO().KeysDown[VK_F1] && ImGui::GetIO().KeysDownDuration[VK_F1] == 0.f)
		{
			if (ImGui::IsPopupOpen("WindowControl"))
			{
				ImGui::CloseCurrentPopup();
			}
			else
			{
				ImGui::OpenPopup("WindowControl");
			}
		}
		if (ImGui::BeginPopup("WindowControl"))
		{
			for (auto& i : myOpenWindows)
			{
				if (ImGui::Checkbox(i.first.c_str(), &i.second))
				{
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}
	}
	bool WindowControl::Window(const char* aLabel, std::function<void()> aContent, ImGuiWindowFlags flags, std::function<void()> aWindowSettings)
	{
		if (myOpenWindows[aLabel])
		{
			PERFORMANCETAG("Imgui_window");
			if (aWindowSettings)
			{
				PERFORMANCETAG("Imgui_setup");
				aWindowSettings();
			}
			if (ImGui::Begin(aLabel, &myOpenWindows[aLabel], flags))
			{
				PERFORMANCETAG("Imgui_content");
				aContent();
			}
			ImGui::End();
			return true;
		}
		return false;
	}

	void WindowControl::SetOpenState(const char* aLabel, bool aState)
	{
		myOpenWindows[aLabel] = aState;
	}

	bool WindowControl::GetWindowState(const char* aLabel)
	{
		return myOpenWindows[aLabel];
	}
}