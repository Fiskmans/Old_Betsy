#ifndef ENGINE_GRAPH_NODE_RENDER_MERGE_NODE_H
#define ENGINE_GRAPH_NODE_RENDER_MERGE_NODE_H

#include "engine/graph/Node.h"
#include "engine/graph/NodePin.h"

#include "engine/graphics/Texture.h"

#include "tools/MathVector.h"

namespace engine::graph::node
{
	class RenderMergeNode : engine::graph::NodeBase
	{
	public:
		void Activate(NodeInstanceId aId) override;

	private:
		InPin<Texture> myInTexture1 = "Tex 1";
		InPin<Texture> myInTexture2 = "Tex 2";
		InPin<tools::V2ui> myInResolution = "Resolution";

		OutPin<Texture> myOutTexture = "Res";

		NodeInstandeDataCollection<Texture> myTextureInstances;
	};

}

#endif