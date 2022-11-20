#include "engine/graph/nodes/RenderDeferredNode.h"

#include "engine/graphics/RenderManager.h"

#include "engine/assets/AssetManager.h"

namespace engine::graph::nodes
{
	void RenderDeferredNode::Activate()
	{

		AssetHandle<DrawableTextureAsset> stashed = myOutTexture.GetOutStorage()->template As<AssetHandle<DrawableTextureAsset>>();
		if (!stashed.IsValid() || myInResolution.GetInPinInstance()->IsDirty())
		{
			if (myInFormat != DXGI_FORMAT_UNKNOWN)
			{
				tools::V2ui resolution = myInResolution;
				if (resolution == tools::V2ui(0, 0))
					resolution = WindowManager::GetInstance().GetSize(); //TODO [2]: register for resolution changes

				stashed = AssetManager::GetInstance().MakeTexture(resolution, myInFormat);
			}
			else
			{
				myOutTexture.GetOutStorage()->MarkRefreshed();
				return;
			}
		}

		AssetHandle<GBufferAsset>& buffer = myInGBuffer;
		if (!buffer.IsValid())
		{
			myOutTexture.GetOutStorage()->MarkRefreshed();
			return;
		}

		buffer.Access().myGBuffer.SetAllAsResources();
		graphics::RenderManager::GetInstance().GetDeferredRenderer().Render(myInCamera);
		myOutTexture = stashed;
	}
}
