#include "pch.h"
#include "WindowControl.h"
#include <Tools\imgui\imgui.h>

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
			if (ImGui::Checkbox(i.first.c_str(),&i.second))
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
		PERFORMANCETAG("Imgui");
		if (aWindowSettings)
		{
			aWindowSettings();
		}
		if (ImGui::Begin(aLabel, &myOpenWindows[aLabel],flags))
		{
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
