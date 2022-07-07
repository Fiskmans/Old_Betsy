#include "engine/graph/NodePin.h"
#include "engine/graph/Node.h"
#include "engine/graph/NodeManager.h"

#include "common/Macros.h"

namespace engine::graph
{

	ImColor ColorFromHashCode(size_t aHashcode)
	{
		float value = aHashcode % 20000;
		return ImColor(0.6f + 0.1f * cos(value), 0.5f + 0.2f * cos(value / 1.8f), 0.5f + 0.3f * cos(value / 3.2f), 1.f);
	}

	void PinBase::Draw(ImVec2 aLocation, bool aHovered)
	{
		const std::type_info& type = Type();

		ImDrawList * drawList = ImGui::GetWindowDrawList();
		ImColor color = ColorFromHashCode(type.hash_code());

		if (aHovered)
			color = ImColor(LERP(color.Value.x, 1.f, 0.5f), LERP(color.Value.y, 1.f, 0.5f), LERP(color.Value.z, 1.f, 0.5f), 1.f);

		const float yOffset = 7.4f;
		const float xOffset = 6.f;

		const float bigCircleSize = 4.1f;
		const float bigCircleThickness = 3.2f;
		const float lineLength = 3.9f;
		const float lineThickness = 1.4f;
		const float smallCirclesize = 2.8f;
		const float smallCircleThickness = 1.9f;

		//drawList->AddRectFilled(aLocation, ImVec2(aLocation.x + 22, aLocation.y + 16), ImColor(0.5, 0.5, 0.5, 1.f));
		drawList->AddCircle(ImVec2(aLocation.x + xOffset, aLocation.y + yOffset), bigCircleSize, color, 0, bigCircleThickness);
		drawList->AddLine(ImVec2(aLocation.x + xOffset + bigCircleSize, aLocation.y + yOffset), ImVec2(aLocation.x + xOffset + bigCircleSize + lineLength, aLocation.y + yOffset), color, lineThickness);
		drawList->AddCircle(ImVec2(aLocation.x + xOffset + bigCircleSize + lineLength + smallCirclesize, aLocation.y + yOffset), smallCirclesize, color, 0, smallCircleThickness);
	}

	//void PinBase::Setup()
	//{
	//	ImGui::DragFloat("yOffset", &yOffset, 0.1f);
	//	ImGui::DragFloat("xOffset", &xOffset, 0.1f);
	//	ImGui::DragFloat("bigCircleSize", &bigCircleSize, 0.1f);
	//	ImGui::DragFloat("bigCircleThickness", &bigCircleThickness, 0.1f);
	//	ImGui::DragFloat("lineLength", &lineLength, 0.1f);
	//	ImGui::DragFloat("lineThickness", &lineThickness, 0.1f);
	//	ImGui::DragFloat("smallCirclesize", &smallCirclesize, 0.1f);
	//	ImGui::DragFloat("smallCircleThickness", &smallCircleThickness, 0.1f);
	//}

	PinValueBase& InPinInstance::Fetch()
	{
		if (myTarget->IsDirty())
			myRefreshCallback();

		return *myTarget;
	}

	namespace node_pin_helpers
	{
		void RegisterInPin(PinBase* aPin)
		{
			NodeManager::GetInstance().AddInPin(aPin);
		}

		void RegisterOutPin(PinBase* aPin)
		{
			NodeManager::GetInstance().AddOutPin(aPin);
		}
	}
}

