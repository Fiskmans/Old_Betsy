#ifndef ENGINE_GRAPH_PIN_H
#define ENGINE_GRAPH_PIN_H

#include "engine/graph/NodeManager.h"

#include "engine/graph/NodePin.h"

namespace engine::graph
{
	class NodeBase
	{
	public:
		NodeBase()
		{
			NodeManager::GetInstance().BeginNode(this);
		}
		virtual ~NodeBase() = default;
		
		virtual void Activate() = 0;

		virtual ImVec2 ImguiSize() { return ImVec2(0, 0); }
		virtual void Imgui(float aScale, ImVec2 aTopLeft) {};
	};
}

#endif