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
		void Activate() override;

	private:
		InPin<Texture> myInTexture = PinInformation("In Texture");

		OutPin<Texture> myOutTexture = PinInformation("Out Texture");
		OutPin<int> myIntPin = PinInformation("strickty pal");
		OutPin<float> myFloatPin = PinInformation("floaty boy");
		OutPin<RenderCopyNode*> myRandom = PinInformation("a node");

		InstancedNodeData<char> mySmall = std::string("char");
		InstancedNodeData<int> myMedium = std::string("int");
		InstancedNodeData<void*> myLarge = std::string("void*");
	};
}

#endif	