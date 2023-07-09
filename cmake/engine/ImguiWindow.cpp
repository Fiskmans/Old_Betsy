
#include "engine/ImGuiWindow.h"

#include "engine/ImGuiManager.h"

#include <typeinfo>

namespace engine
{
	ImGuiWindow::ImGuiWindow()
	{
		ImGuiManager::GetInstance().AddWindow(this);
	}

	ImGuiWindow::~ImGuiWindow()
	{
		ImGuiManager::GetInstance().RemoveWindow(this);
	}
}