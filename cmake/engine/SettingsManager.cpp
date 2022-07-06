
#include "engine/SettingsManager.h"

#include "imgui/imgui.h"

#include <fstream>

namespace engine
{
	void SettingsManager::SaveImGuiStyle()
	{
		static_assert(sizeof(char) == 1, "Double check");
		std::ofstream stream;
		stream.open("ImGuiStyle.ini", std::ios::binary | std::ios::out);
		stream.write(reinterpret_cast<char*>(&ImGui::GetStyle()), sizeof(*(&ImGui::GetStyle())));
	}

	void SettingsManager::LoadOrDefaultImGuiStyle()
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
}