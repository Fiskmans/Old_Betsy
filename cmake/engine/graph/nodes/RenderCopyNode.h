#ifndef ENGINE_GRAPH_NODE_RENDER_COPY_NODE_H
#define ENGINE_GRAPH_NODE_RENDER_COPY_NODE_H

#include "engine/graph/Node.h"
#include "engine/graph/NodePin.h"

#include "engine/graphics/Texture.h"

namespace engine::graph::node
{
	class RenderCopyNode : public NodeBase<RenderCopyNode>
	{
	public:


	private:
		InPin<Texture> myInTexture = "In Texture";
		OutPin<Texture> myOutTexture = "Out Texture";
	};
}

#endif	