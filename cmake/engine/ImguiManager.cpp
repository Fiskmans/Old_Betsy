#include "engine/ImguiManager.h"

#include "engine/graphics/GraphicEngine.h"
#include "engine/graphics/WindowManager.h"

#include "imgui/CustomWidgets.h"
#include "imgui/backend/imgui_impl_dx11.h"
#include "imgui/backend/imgui_impl_win32.h"
#include "imgui/imgui.h"

namespace engine
{
	void ImGuiManager::Init()
	{
		ImGui_ImplDX11_Init(graphics::GraphicsEngine::GetInstance().GetFrameWork().GetDevice(), graphics::GraphicsEngine::GetInstance().GetFrameWork().GetContext());
		ImGui_ImplWin32_Init(WindowManager::GetInstance().GetWindowHandle());
	}

	void ImGuiManager::Update()
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		
		if (ImGui::BeginMainMenuBar())
		{
			for (auto& category : myWindows)
			{
				if (ImGui::BeginMenu(category.first.c_str()))
				{
					ImGui::MenuItem(category.first.c_str(), nullptr, false, false);
					ImGui::Separator();

					for (ImGuiWindow* window : category.second)
					{
						ImGui::MenuItem(window->ImGuiName(), nullptr, &window->myImGuiOpen);
					}

					ImGui::EndMenu();
				}
			}

			ImGui::EndMainMenuBar();
		}
		for (auto& category : myWindows)
		{
			for (ImGuiWindow* window : category.second)
			{
				if (!window->myImGuiOpen)
					continue;

				if (ImGui::Begin(window->ImGuiName(), &window->myImGuiOpen))
				{
					window->OnImgui();
				}
				ImGui::End();
			}
		}

		ImGui::Render();

		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	void ImGuiManager::AddWindow(const std::string& aCategory, ImGuiWindow* aWindow)
	{
		myWindows[aCategory].push_back(aWindow);
	}

	void ImGuiManager::RemoveWindow(ImGuiWindow* aWindow)
	{
		for (auto& category : myWindows)
		{
			std::vector<ImGuiWindow*>::iterator it = std::find(category.second.begin(), category.second.end(), aWindow);

			if (it != category.second.end())
			{
				category.second.erase(it);
			}

		}
	}

}

