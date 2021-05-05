#include "pch.h"
#include "GBuffer.h"
#include <d3d11.h>
#include "FullscreenTexture.h"

GBuffer::~GBuffer()
{
	//for (size_t i = 0; i < ENUM_CAST(Textures::Count); i++) TODO: move to dispose
	//{
	//	myTextures[i]->Release();
	//	myRenderTargets[i]->Release();
	//	myShaderResources[i]->Release();
	//
	//	myTextures[i] = nullptr;
	//	myRenderTargets[i] = nullptr;
	//	myShaderResources[i] = nullptr;
	//}
	//
	//myContext = nullptr;
	//myViewport = nullptr; //?
}

void GBuffer::ClearTextures(const V4F aClearColor)
{
	for (auto& i : myRenderTargets)
	{
		myContext->ClearRenderTargetView(i, &aClearColor.x);
	}
}

void GBuffer::SetAsActiveTarget(FullscreenTexture* aDepth)
{
	myContext->OMSetRenderTargets(ENUM_CAST(GBuffer::Textures::Count), myRenderTargets.data(), aDepth ? aDepth->myDepth : nullptr);
	myContext->RSSetViewports(1, myViewport);
}

void GBuffer::SetAsActiveTarget(const Textures aResource, class FullscreenTexture* aDepth)
{
	myContext->OMSetRenderTargets(1, myRenderTargets.data()+static_cast<int>(aResource), aDepth ? aDepth->myDepth : nullptr);
	myContext->RSSetViewports(1, myViewport);
}

void GBuffer::SetAsResourceOnSlot(const Textures aResource, const unsigned int aSlor)
{
	myContext->PSSetShaderResources(aSlor, 1, &myShaderResources[ENUM_CAST(aResource)]);
}

void GBuffer::SetAllAsResources()
{
	myContext->PSSetShaderResources(0, ENUM_CAST(GBuffer::Textures::Count), myShaderResources.data());
}

void GBuffer::CopyTo(GBuffer* aOther, ID3D11DeviceContext* aContext)
{
	for (size_t i = 0; i < ENUM_CAST(Textures::Count); i++)
	{
		aContext->CopyResource(aOther->myTextures[i], myTextures[i]);
	}
}

void GBuffer::Release()
{
	for (auto& i : myRenderTargets)
	{
		SAFE_RELEASE(i);
	}
	for (auto& i : myShaderResources)
	{
		SAFE_RELEASE(i);
	}
	for (auto& i : myTextures)
	{
		SAFE_RELEASE(i);
	}
	SAFE_DELETE(myViewport);
}

GBuffer::GBuffer()
{
	WIPE(*this);
}
