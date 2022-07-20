#ifndef ENGINE_GRAPH_NODE_RENDER_MERGE_NODE_H
#define ENGINE_GRAPH_NODE_RENDER_MERGE_NODE_H

#include "engine/graph/Node.h"
#include "engine/graph/NodePin.h"

#include "engine/graphics/Texture.h"

#include "engine/assets/Asset.h"

#include "tools/MathVector.h"

#include <d3d11.h>

namespace engine::graph::node
{
	class RenderMergeNode : engine::graph::NodeBase
	{
	public:
		void Activate(NodeInstanceId aId) override;

		ImVec2 ImguiSize(NodeInstanceId aId) override { return ImVec2(40, 40); }
		void Imgui(NodeInstanceId aId, float aScale, ImVec2 aTopLeft) override;

	private:
		InPin<AssetHandle> myInTexture1 = "Tex 1";
		InPin<AssetHandle> myInTexture2 = "Tex 2";
		InPin<tools::V2ui> myInResolution = "Resolution";
		InPin<DXGI_FORMAT> myInFormat = "Format";

		OutPin<AssetHandle> myOutTexture = "Res";
	};

}

#endif