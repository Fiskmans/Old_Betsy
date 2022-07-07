#ifndef ENGINE_GRAPH_NODE_REGISTRATION_H
#define ENGINE_GRAPH_NODE_REGISTRATION_H

#include "tools/Singleton.h"

#include "engine/graph/nodes/RenderCopyNode.h"

namespace engine::graph
{
	class NodeRegistration : public tools::Singleton<NodeRegistration>
	{
	public:
		static void Register()
		{
			GetInstance();
		}

	private:
		node::RenderCopyNode renderCopyNode;
	};
}

#endif