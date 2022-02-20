#include "pch.h"
#include "Texture.h"
#include <d3d11.h>

void Texture::Release()
{
	SAFE_RELEASE(myTexture);
	SAFE_RELEASE(myShaderResource);
	SAFE_DELETE(myViewport);
	SAFE_RELEASE(myDepth);
	SAFE_RELEASE(myRenderTarget);
}

void Texture::ClearTexture(V4F aClearColor)
{
	myContext->ClearRenderTargetView(myRenderTarget, &aClearColor.x);
}

void Texture::ClearDepth(float aClearDepth, unsigned int aClearStencil)
{
	myContext->ClearDepthStencilView(myDepth, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, aClearDepth, static_cast<UINT8>(aClearStencil));
}

void Texture::SetAsActiveTarget(Texture* aDepth)
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

UpdatableTexture::UpdatableTexture(CommonUtilities::Vector2<unsigned int> aSize)
{
	mySize = aSize;
	myData.resize(mySize.x * mySize.y);
}

void UpdatableTexture::Upload()
{
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	if (FAILED(myContext->Map(myTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource)))
	{
		SYSWARNING("Failed to map updatable texture");
		return;
	}
	for (unsigned int row = 0; row < mySize.y; row++)
	{
		memcpy(static_cast<char*>(mappedSubresource.pData) + mappedSubresource.RowPitch * row, myData.data() + row * mySize.x, mySize.x * sizeof(float));
	}
	myContext->Unmap(myTexture, 0);
}

float UpdatableTexture::Sample(unsigned int aX, unsigned int aY)
{
	return myData[aY * mySize.x + aX];
}
