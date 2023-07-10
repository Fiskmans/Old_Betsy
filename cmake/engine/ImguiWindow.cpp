
#include "engine/ImGuiWindow.h"

#include "engine/ImGuiManager.h"

#include <typeinfo>

namespace engine
{
	ImGuiWindow::ImGuiWindow(const std::string& aCategory)
	{
		ImGuiManager::GetInstance().AddWindow(aCategory, this);
	}

	ImGuiWindow::~ImGuiWindow()
	{
		ImGuiManager::GetInstance().RemoveWindow(this);
	}
}