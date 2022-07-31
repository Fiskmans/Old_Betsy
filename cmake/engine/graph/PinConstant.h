#ifndef ENGINE_GRAPH_PIN_CONSTANT_H
#define ENGINE_GRAPH_PIN_CONSTANT_H

#include "engine/graph/PinValue.h"

#include "tools/ImGuiHelpers.h"

#include <string>

#include <d3d11.h>

namespace engine::graph
{
	namespace pin_constant_helpers
	{
		template<class Type>
		class EditableField
		{
		public:
			static ImVec2 Size(Type& aValue) { return ImVec2(0, 0); };
			static bool Imgui(float aScale, ImVec2 aLocation, Type& aValue) { return false; };
		};
	}


	class Dependable;

	template<class Type>
	class PinConstant : public PinValueBase
	{
	public:
		PinConstant(Dependable* aChild);
		void Unlink() override { delete this; }
		bool IsConstant() override { return true; }

		ImVec2 ImGuiSize() override { return pin_constant_helpers::EditableField<Type>::Size(myStorage); }
		void ImGui(float aScale, ImVec2 aLocation) override { if (pin_constant_helpers::EditableField<Type>::Imgui(aScale, aLocation, myStorage)) MarkRefreshed(); }

	private:
		void* GetRaw() override { return &myStorage; }
		const void* GetRaw() const override { return &myStorage; }

		Type myStorage;
	};

	template<class Type>
	PinConstant<Type>::PinConstant(Dependable* aChild)
		: myStorage{}
		
	{
		AddDependent(aChild);
		SetRefreshCallback([]() {});
		MarkRefreshed();
	}


	//////////////////////////////////////////////////////////////////////////

	namespace pin_constant_helpers
	{
		template<>
		class EditableField<int>
		{
		public:
			static ImVec2 Size(int& aValue) { return ImVec2(45 + ImGui::CalcTextSize(std::to_string(aValue).c_str()).x, 20); };
			static bool Imgui(float aScale, ImVec2 aLocation, int& aValue) 
			{
				bool changed = false;
				ImDrawList* drawlist = ImGui::GetWindowDrawList();

				ImVec2 pos = ImVec2(aLocation.x + 1.f * aScale, aLocation.y);
				ImGui::SetCursorScreenPos(pos);
				if (ImGui::InvisibleButton("-", ImVec2(20.f * aScale, 20.f * aScale)))
				{
					aValue--;
					changed = true;
				}

				drawlist->AddRectFilled(pos, ImVec2(pos.x + 20.f * aScale, pos.y + 20.f * aScale), tools::GetImColor(ImGui::IsItemHovered() ? ImGuiCol_ButtonHovered : ImGuiCol_Button));
				drawlist->AddText(ImGui::GetFont(), ImGui::GetFontSize() * aScale, ImVec2(pos.x + 6.f * aScale, pos.y + 4.f * aScale), tools::GetImColor(ImGuiCol_Text), "-");

				pos.x += 23.f * aScale;

				ImGui::SetCursorScreenPos(ImVec2(aLocation.x + 24.f * aScale, aLocation.y));
				if (ImGui::InvisibleButton("+", ImVec2(20.f * aScale, 20.f * aScale)))
				{
					aValue++;
					changed = true;
				}
				drawlist->AddRectFilled(pos, ImVec2(pos.x + 20.f * aScale, pos.y + 20.f * aScale), tools::GetImColor(ImGui::IsItemHovered() ? ImGuiCol_ButtonHovered : ImGuiCol_Button));
				drawlist->AddText(ImGui::GetFont(), ImGui::GetFontSize() * aScale, ImVec2(pos.x + 6.f * aScale, pos.y + 4.f * aScale), tools::GetImColor(ImGuiCol_Text), "+");

				pos.x += 21.f * aScale;
				drawlist->AddText(ImGui::GetFont(), ImGui::GetFontSize()* aScale, ImVec2(pos.x + 6.f * aScale, pos.y + 4.f * aScale), tools::GetImColor(ImGuiCol_Text), std::to_string(aValue).c_str());

				return changed;
			};
		};


		template<>
		class EditableField<DXGI_FORMAT>
		{
		public:
			static ImVec2 Size(DXGI_FORMAT& aValue) { return ImVec2(10.f * 5.f, 10.f); };
			static bool Imgui(float aScale, ImVec2 aLocation, DXGI_FORMAT& aValue)
			{
				bool changed = false;
				ImDrawList* drawlist = ImGui::GetWindowDrawList();

				ImGui::SetCursorScreenPos(ImVec2(aLocation.x, aLocation.y));

				size_t channels = 0;

				switch (aValue)
				{
				case DXGI_FORMAT_R16_FLOAT:
					channels = 1;
					break;
				case DXGI_FORMAT_R16G16_FLOAT:
					channels = 2;
					break;
				case DXGI_FORMAT_R16G16B16A16_FLOAT:
					channels = 3;
					break;
				default:
					break;
				}

				ImVec2 pos = aLocation;
				ImGui::SetCursorScreenPos(pos);
				if (ImGui::InvisibleButton("down", ImVec2(10.f * aScale, 10.f * aScale)) && channels > 1)
				{
					changed = true;
					channels--;
				}
				drawlist->AddRectFilled(pos, ImVec2(pos.x + 10 * aScale, pos.y + 10 * aScale), tools::GetImColor(ImGui::IsItemHovered() ? ImGuiCol_ButtonHovered : ImGuiCol_Button), 2.f * aScale);
				pos.x += 10.f * aScale;

				float activeIntensity = 1.f;
				float inactiveIntensity = 0.4f;
				float passiveInstesity = 0.2f;

				drawlist->AddRectFilled(pos, ImVec2(pos.x + 10 * aScale, pos.y + 10 * aScale), ImColor(channels > 0 ? activeIntensity : inactiveIntensity, passiveInstesity, passiveInstesity, 1.f), 2.f * aScale);
				pos.x += 10.f * aScale;
				drawlist->AddRectFilled(pos, ImVec2(pos.x + 10 * aScale, pos.y + 10 * aScale), ImColor(passiveInstesity, channels > 1 ? activeIntensity : inactiveIntensity, passiveInstesity, 1.f), 2.f * aScale);
				pos.x += 10.f * aScale;
				drawlist->AddRectFilled(pos, ImVec2(pos.x + 10 * aScale, pos.y + 10 * aScale), ImColor(passiveInstesity, passiveInstesity, channels > 2 ? activeIntensity : inactiveIntensity, 1.f), 2.f * aScale);
				pos.x += 10.f * aScale;

				if (channels == 0)
					drawlist->AddLine(ImVec2(aLocation.x + 10.f * aScale, aLocation.y), ImVec2(pos.x, pos.y + 10.f * aScale), ImColor(0.7f, 0.7f, 0.7f, 1.f), 1.f * aScale);

				ImGui::SetCursorScreenPos(pos);
				if (ImGui::InvisibleButton("up", ImVec2(10.f * aScale, 10.f * aScale)) && channels < 3)
				{
					changed = true;
					channels++;
				}
				drawlist->AddRectFilled(pos, ImVec2(pos.x + 10 * aScale, pos.y + 10 * aScale), tools::GetImColor(ImGui::IsItemHovered() ? ImGuiCol_ButtonHovered : ImGuiCol_Button), 2.f * aScale);
				pos.x += 10.f * aScale;

				if (changed)
				{

					const DXGI_FORMAT formatMapping[] =
					{
						DXGI_FORMAT_R16_FLOAT,
						DXGI_FORMAT_R16G16_FLOAT,
						DXGI_FORMAT_R16G16B16A16_FLOAT
					};
					aValue = formatMapping[channels - 1];
				}

				return changed;
			};
		};
	}
}

#endif