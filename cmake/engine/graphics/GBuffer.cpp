#include "engine/graphics/GBuffer.h"

#include "common/Macros.h"

namespace engine
{
	void GBuffer::ClearTextures(const tools::V4f aClearColor)
	{
		for (auto& i : myRenderTargets)
		{
			myContext->ClearRenderTargetView(i, aClearColor.Raw());
		}
	}

	void GBuffer::SetAsActiveTarget(Texture* aDepth)
	{
		myContext->OMSetRenderTargets(static_cast<int>(GBuffer::Textures::Count), myRenderTargets.data(), aDepth ? aDepth->myDepth : nullptr);
		myContext->RSSetViewports(1, myViewport);
	}

	void GBuffer::SetAsActiveTarget(const Textures aResource, class Texture* aDepth)
	{
		myContext->OMSetRenderTargets(1, myRenderTargets.data() + static_cast<int>(aResource), aDepth ? aDepth->myDepth : nullptr);
		myContext->RSSetViewports(1, myViewport);
	}

	void GBuffer::SetAsResourceOnSlot(const Textures aResource, const unsigned int aSlor)
	{
		myContext->PSSetShaderResources(aSlor, 1, &myShaderResources[static_cast<int>(aResource)]);
	}

	void GBuffer::SetAllAsResources()
	{
		myContext->PSSetShaderResources(0, static_cast<int>(GBuffer::Textures::Count), myShaderResources.data());
	}

	void GBuffer::CopyTo(GBuffer* aOther, ID3D11DeviceContext* aContext)
	{
		for (size_t i = 0; i < static_cast<int>(Textures::Count); i++)
		{
			aContext->CopyResource(aOther->myTextures[i], myTextures[i]);
		}
	}

	void GBuffer::Release()
	{
		for (ID3D11Texture2D*& i : myTextures)
		{
			SAFE_RELEASE(i);
		}
		for (ID3D11RenderTargetView*& i : myRenderTargets)
		{
			SAFE_RELEASE(i);
		}
		for (ID3D11ShaderResourceView*& i : myShaderResources)
		{
			SAFE_RELEASE(i);
		}
		SAFE_DELETE(myViewport);
	}
}