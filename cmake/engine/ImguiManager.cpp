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
			if (ImGui::BeginMenu("Windows", "W"))
			{
				ImGui::MenuItem("Windows", nullptr, false, false);
				ImGui::Separator();

				for (ImGuiWindow* window : myWindows)
				{
					ImGui::MenuItem(window->ImGuiName(), nullptr, &window->myImGuiOpen);
				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		for (ImGuiWindow* window : myWindows)
		{
			if (!window->myImGuiOpen)
				continue;

			if (ImGui::Begin(window->ImGuiName()))
			{
				window->OnImgui();
			}
			ImGui::End();
		}

		ImGui::Render();

		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	void ImGuiManager::AddWindow(ImGuiWindow* aWindow)
	{
		myWindows.push_back(aWindow);
	}

	void ImGuiManager::RemoveWindow(ImGuiWindow* aWindow)
	{
		decltype(myWindows)::iterator it = std::find(myWindows.begin(), myWindows.end(), aWindow);

		if (it != myWindows.end())
		{
			myWindows.erase(it);
		}
	}

}

