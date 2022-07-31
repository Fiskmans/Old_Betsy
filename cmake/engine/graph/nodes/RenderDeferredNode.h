#ifndef ENGINE_GRAPH_NODES_RENDER_DEFERRED_H
#define ENGINE_GRAPH_NODES_RENDER_DEFERRED_H

#include "engine/graphics/GBuffer.h"
#include "engine/graphics/Camera.h"

#include "engine/assets/Asset.h"

#include "engine/graph/Node.h"

#include "tools/MathVector.h"

#include <d3d11.h>

namespace engine::graph::nodes
{
	class RenderDeferredNode : public NodeBase
	{
	public:

		void Activate() override;

	private:
		InPin<graphics::GBuffer> myInGBuffer = PinInformation("Geometry");
		InPin<Camera*> myInCamera = PinInformation("Camera");
		InPin<tools::V2ui> myInResolution = PinInformation("Resolution");
		InPin<DXGI_FORMAT> myInFormat = PinInformation("Format");


		OutPin<AssetHandle> myOutTexture = PinInformation("Tex");
	};
}

#endif