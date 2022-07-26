#ifndef ENGINE_GRAPH_NODE_REGISTRATION_H
#define ENGINE_GRAPH_NODE_REGISTRATION_H

#include "tools/Singleton.h"

#include "engine/graph/nodes/RenderCopyNode.h"
#include "engine/graph/nodes/RenderMergeNode.h"
#include "engine/graph/nodes/TestNodes.h"
#include "engine/graph/nodes/ImageNode.h"
#include "engine/graph/Nodes/MulNode.h"

#include "common/Macros.h"

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
		node::RenderCopyNode UNIQUENAME;
		node::RenderMergeNode UNIQUENAME;
		node::PrintNode UNIQUENAME;
		node::IntConstant UNIQUENAME;
		node::IntPassThrough UNIQUENAME;

		node::ImageNode UNIQUENAME;
		node::EngineTextureNode UNIQUENAME;
		node::MulNode UNIQUENAME;
	};
}

#endif