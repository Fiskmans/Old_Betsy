#ifndef ENGINE_GRAPH_NODE_RENDER_COPY_NODE_H
#define ENGINE_GRAPH_NODE_RENDER_COPY_NODE_H

#include "engine/graph/Node.h"
#include "engine/graph/NodePin.h"

#include "engine/graphics/Texture.h"

namespace engine::graph::node
{
	class RenderCopyNode : public NodeBase
	{
	public:
		void Activate(NodeInstanceId aId) override;

	private:
		InPin<Texture> myInTexture = "In Texture";

		OutPin<Texture> myOutTexture = "Out Texture";
		OutPin<int> myIntPin = "strickty pal";
		OutPin<float> myFloatPin = "floaty boy";
		OutPin<RenderCopyNode*> myRandom = "a node";
	};
}

#endif	