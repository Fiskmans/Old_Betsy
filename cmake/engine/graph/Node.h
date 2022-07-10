#ifndef ENGINE_GRAPH_PIN_H
#define ENGINE_GRAPH_PIN_H

#include "engine/graph/NodeManager.h"

#include "tools/Singleton.h"


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
		
		virtual void Activate(NodeInstanceId aId) = 0;

		virtual ImVec2 ImguiSize(NodeInstanceId aId) { return ImVec2(0, 0); }
		virtual void Imgui(NodeInstanceId aId, float aScale, ImVec2 aTopLeft) {};
	};
}

#endif