
#include "engine/graphics/Texture.h"
#include "engine/graphics/GraphicEngine.h"

#include "common/Macros.h"

#include "logger/Logger.h"

namespace engine::graphics
{
	void Texture::Release()
	{
		SAFE_RELEASE(myTexture);
		SAFE_RELEASE(myShaderResource);
		SAFE_DELETE(myViewport);
		SAFE_RELEASE(myRenderTarget);
	}

	void Texture::ClearTexture(tools::V4f aClearColor)
	{
		ID3D11DeviceContext* context = GraphicsEngine::GetInstance().GetFrameWork().GetContext();
		context->ClearRenderTargetView(myRenderTarget, aClearColor.Raw());
	}

	void Texture::SetAsActiveTarget(const DepthTexture* aDepth) const
	{
		ID3D11DeviceContext* context = GraphicsEngine::GetInstance().GetFrameWork().GetContext();
		context->OMSetRenderTargets(1, &myRenderTarget, aDepth ? aDepth->myDepth : nullptr);
		context->RSSetViewports(1, myViewport);
	}

	void Texture::SetAsResourceOnSlot(unsigned int aSlot)
	{
		ID3D11DeviceContext* context = GraphicsEngine::GetInstance().GetFrameWork().GetContext();
		context->PSSetShaderResources(aSlot, 1, &myShaderResource);
	}

	ID3D11ShaderResourceView* Texture::GetResourceView()
	{
		return myShaderResource;
	}

	void DepthTexture::ClearDepth(float aClearDepth, unsigned int aClearStencil)
	{
		ID3D11DeviceContext* context = GraphicsEngine::GetInstance().GetFrameWork().GetContext();
		context->ClearDepthStencilView(myDepth, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, aClearDepth, static_cast<UINT8>(aClearStencil));
	}

	void DepthTexture::Release()
	{
		myDepth->Release();
	}

	graphics::UpdatableTexture::UpdatableTexture()
	{
	}

	UpdatableTexture::UpdatableTexture(tools::V2ui aSize)
		: mySize(aSize)
	{
		myData.resize(mySize[0] * mySize[0]);
	}

	void graphics::UpdatableTexture::DrawDirect(size_t x, size_t y, float* aData, size_t length)
	{
		memcpy(myData.data() + x + y * mySize[1], aData, length * sizeof(float));
	}

	void UpdatableTexture::Upload()
	{
		ID3D11DeviceContext* context = GraphicsEngine::GetInstance().GetFrameWork().GetContext();
		D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		HRESULT result = context->Map(myTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
		if (FAILED(result))
		{
			LOG_SYS_ERROR("Failed to map updatable texture");
			return;
		}
		for (unsigned int row = 0; row < mySize[1]; row++)
		{
			memcpy(static_cast<char*>(mappedSubresource.pData) + mappedSubresource.RowPitch * row, myData.data() + row * mySize[0], mySize[0] * sizeof(float));
		}
		context->Unmap(myTexture, 0);
	}

	float UpdatableTexture::Sample(tools::V2ui aPosition)
	{
		return myData[aPosition[1] * mySize[0] + aPosition[0]];
	}
}

