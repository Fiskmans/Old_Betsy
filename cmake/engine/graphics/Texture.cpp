
#include "engine/graphics/Texture.h"

#include "common/Macros.h"

#include "logger/Logger.h"

namespace engine
{
	void Texture::Release()
	{
		SAFE_RELEASE(myTexture);
		SAFE_RELEASE(myShaderResource);
		SAFE_DELETE(myViewport);
		SAFE_RELEASE(myDepth);
		SAFE_RELEASE(myRenderTarget);
	}

	void Texture::ClearTexture(tools::V4f aClearColor)
	{
		myContext->ClearRenderTargetView(myRenderTarget, aClearColor.Raw());
	}

	void Texture::ClearDepth(float aClearDepth, unsigned int aClearStencil)
	{
		myContext->ClearDepthStencilView(myDepth, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, aClearDepth, static_cast<UINT8>(aClearStencil));
	}

	void Texture::SetAsActiveTarget(Texture* aDepth) const
	{
		if (aDepth)
		{
			myContext->OMSetRenderTargets(1, &myRenderTarget, aDepth->myDepth);
		}
		else
		{
			myContext->OMSetRenderTargets(1, &myRenderTarget, nullptr);
		}
		myContext->RSSetViewports(1, myViewport);
	}

	void Texture::SetAsResourceOnSlot(unsigned int aSlot)
	{
		myContext->PSSetShaderResources(aSlot, 1, &myShaderResource);
	}

	ID3D11ShaderResourceView* Texture::GetResourceView()
	{
		return myShaderResource;
	}

	UpdatableTexture::UpdatableTexture(tools::V2ui aSize)
		: mySize(aSize)
	{
		myData.resize(mySize[0] * mySize[0]);
	}

	void UpdatableTexture::Upload()
	{
		D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		if (FAILED(myContext->Map(myTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource)))
		{
			LOG_SYS_ERROR("Failed to map updatable texture");
			return;
		}
		for (unsigned int row = 0; row < mySize[1]; row++)
		{
			memcpy(static_cast<char*>(mappedSubresource.pData) + mappedSubresource.RowPitch * row, myData.data() + row * mySize[0], mySize[0] * sizeof(float));
		}
		myContext->Unmap(myTexture, 0);
	}

	float UpdatableTexture::Sample(tools::V2ui aPosition)
	{
		return myData[aPosition[1] * mySize[0] + aPosition[0]];
	}
}