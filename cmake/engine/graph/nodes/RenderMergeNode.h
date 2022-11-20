#ifndef ENGINE_GRAPH_NODES_RENDER_MERGE_NODE_H
#define ENGINE_GRAPH_NODES_RENDER_MERGE_NODE_H

#include "engine/graph/Node.h"
#include "engine/graph/NodePin.h"

#include "engine/graphics/Texture.h"

#include "engine/assets/Asset.h"

#include "tools/MathVector.h"

#include <d3d11.h>

namespace engine::graph::nodes
{
	class RenderMergeNode : engine::graph::NodeBase
	{
	public:
		void Activate() override;

		ImVec2 ImguiSize() override { return ImVec2(40, 40); }
		void Imgui(float aScale, ImVec2 aTopLeft) override;

	private:
		InPin<AssetHandle<TextureAsset>> myInTexture1 = PinInformation("Tex 1");
		InPin<AssetHandle<TextureAsset>> myInTexture2 = PinInformation("Tex 2");
		InPin<tools::V2ui> myInResolution = PinInformation("Resolution");
		InPin<DXGI_FORMAT> myInFormat = PinInformation("Format");

		OutPin<AssetHandle<DrawableTextureAsset>> myOutTexture = PinInformation("Res");
	};

}

#endif