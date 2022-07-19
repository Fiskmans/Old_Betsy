#ifndef ENGINE_GRAPH_PIN_CONSTANT_H
#define ENGINE_GRAPH_PIN_CONSTANT_H

#include "engine/graph/PinValue.h"

#include <string>

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
		Type myStorage;
	};

	template<class Type>
	PinConstant<Type>::PinConstant(Dependable* aChild)
		: PinValueBase(myStorage)
		, myStorage{}
		
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
				//ImDrawList* drawlist = ImGui::GetWindowDrawList();

				ImGui::SetCursorScreenPos(ImVec2(aLocation.x, aLocation.y));
				if (ImGui::Button("+", ImVec2(20, 20)))
				{
					aValue++;
					changed = true;
				}

				ImGui::SameLine();
				if (ImGui::Button("-", ImVec2(20, 20)))
				{
					aValue--;
					changed = true;
				}

				return changed;
			};
		};
	}
}

#endif