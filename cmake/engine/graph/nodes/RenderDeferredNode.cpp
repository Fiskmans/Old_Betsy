#include "engine/graph/nodes/RenderDeferredNode.h"

#include "engine/graphics/RenderManager.h"

#include "engine/assets/AssetManager.h"

namespace engine::graph::nodes
{
	void RenderDeferredNode::Activate()
	{

		AssetHandle stashed = myOutTexture.GetOutStorage()->template As<AssetHandle>();
		if (!stashed.IsValid() || myInResolution.GetInPinInstance()->IsDirty())
		{
			if (myInFormat != DXGI_FORMAT_UNKNOWN && myInResolution != tools::V2ui(0, 0))
			{
				stashed = AssetManager::GetInstance().MakeTexture(myInResolution, myInFormat);
			}
			else
			{
				myOutTexture.GetOutStorage()->MarkRefreshed();
				return;
			}
		}

		graphics::GBuffer& buffer = myInGBuffer;
		if (!buffer.IsValid())
		{
			myOutTexture.GetOutStorage()->MarkRefreshed();
			return;
		}

		buffer.SetAllAsResources();
		graphics::RenderManager::GetInstance().GetDeferredRenderer().Render(myInCamera);
		myOutTexture = stashed;
	}
}
