#ifndef ENGINE_GRAPH_DRAWABLE_PIN_BLOCK_H
#define ENGINE_GRAPH_DRAWABLE_PIN_BLOCK_H

#include "imgui/imgui.h"

#include <vector>

namespace engine::graph
{
	class Graph;
	class PinBase;

	class DrawablePinBlock
	{
	public:
		DrawablePinBlock(ImVec2 aPosition);

		virtual ~DrawablePinBlock() = default;

		bool Imgui(const char* aName, Graph* aGraph,float aScale, ImVec2 aPosition, const std::vector<PinBase*>& aInPins, const std::vector<PinBase*>& aOutPins);

		virtual ImVec2 CustomImguiSize() { return ImVec2(0, 0); }
		virtual void CustomImgui(float aScale, ImVec2 aTopLeft) { }

		void Move(ImVec2 aDelta);

		ImVec2 myPosition;
		bool myIsMoving = false;
	};
}

#endif