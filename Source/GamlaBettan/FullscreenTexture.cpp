#include "pch.h"
#include "FullscreenTexture.h"
#include <d3d11.h>

void FullscreenTexture::Release()
{
	SAFE_RELEASE(myTexture);
	SAFE_RELEASE(myShaderResource);
	SAFE_DELETE(myViewport);
	SAFE_RELEASE(myDepth);
	SAFE_RELEASE(myRenderTarget);
}

void FullscreenTexture::ClearTexture(V4F aClearColor)
{
	myContext->ClearRenderTargetView(myRenderTarget, &aClearColor.x);
}

void FullscreenTexture::ClearDepth(float aClearDepth, unsigned int aClearStencil)
{
	myContext->ClearDepthStencilView(myDepth, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, aClearDepth, aClearStencil);
}

void FullscreenTexture::SetAsActiveTarget(FullscreenTexture* aDepth)
{
	if (aDepth)
	{
		myContext->OMSetRenderTargets(1, &myRenderTarget, aDepth->myDepth);
	}
	else
	{
		myContext->OMSetRenderTargets(1, &myRenderTarget,nullptr);
	}
	myContext->RSSetViewports(1, myViewport);
}

void FullscreenTexture::SetAsResourceOnSlot(unsigned int aSlot)
{
	myContext->PSSetShaderResources(aSlot, 1, &myShaderResource);
}

ID3D11ShaderResourceView* FullscreenTexture::GetResourceView()
{
	return myShaderResource;
}