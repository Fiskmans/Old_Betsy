#include "pch.h"
#include "ShadowRenderer.h"
#include "CameraFactory.h"
#include "DirectX11Framework.h"
#include "PointLight.h"
#include "Model.h"
#include "ShaderTypes.h"
#include "ShaderFlags.h"
#include "ShaderBuffers.h"
#include "Shaders.h"
#include "ShaderCompiler.h"


bool ShadowRenderer::Init(DirectX11Framework* aFramework)
{
	ID3D11Device* device = aFramework->GetDevice();
	myContext = aFramework->GetContext();
	for (size_t i = 0; i < 6; i++)
	{
		myCameras[i] = CCameraFactory::CreateCamera(90, false,SHADOWNEARPLANE,SHADOWFARPLANE);
		myCameras[i]->SetResolution(V2F(512, 512));
	}
	myCameras[0]->SetRotation(V3F(0, 0, 0));
	myCameras[1]->SetRotation(V3F(PI, 0, 0));
	myCameras[2]->SetRotation(V3F(-PI / 2, 0,0));
	myCameras[3]->SetRotation(V3F(PI / 2, 0, 0));
	myCameras[4]->SetRotation(V3F(0, -PI/2, 0));
	myCameras[5]->SetRotation(V3F(0, PI/2, 0));


	ID3D11Texture2D* tartex;
	CD3D11_TEXTURE2D_DESC desc;
	WIPE(desc);
	desc.BindFlags = D3D11_BIND_RENDER_TARGET;
	desc.Usage = D3D11_USAGE_DEFAULT;

	desc.Format = DXGI_FORMAT_R32_FLOAT;
	desc.Height = SHADOWRESOLUTION;
	desc.Width = SHADOWRESOLUTION*6;
	desc.ArraySize = 1;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	HRESULT result;

	myShader = GetPixelShader(device,"Data/Shaders/White.hlsl",ShaderFlags::None);
	if (!myShader)
	{
		SYSERROR("could not compile White shader for shadowrender.");
		return false;
	}

	ID3D11Texture2D* tex;
	ID3D11ShaderResourceView* view;
	ID3D11DepthStencilView* depth;

	CD3D11_TEXTURE2D_DESC tdesc;
	WIPE(tdesc);
	tdesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	tdesc.Usage = D3D11_USAGE_DEFAULT;

	tdesc.Format = DXGI_FORMAT_R32_TYPELESS;
	tdesc.Height = SHADOWRESOLUTION;
	tdesc.Width = SHADOWRESOLUTION * 6;
	tdesc.ArraySize = 1;
	tdesc.MipLevels = 1;
	tdesc.SampleDesc.Count = 1;
	tdesc.SampleDesc.Quality = 0;

	result = device->CreateTexture2D(&tdesc, nullptr, &tex);
	if (FAILED(result))
	{
		SYSERROR("Could not shadow depth texture");
		return false;
	}


	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	WIPE(dsvDesc);
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;

	result = device->CreateDepthStencilView(tex, &dsvDesc, &depth);
	if (FAILED(result))
	{
		SYSERROR("Could not shadow depth view");
		return false;
	}


	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	WIPE(srvDesc);
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	result = device->CreateShaderResourceView(tex, &srvDesc, &view);
	tex->Release();
	if (FAILED(result))
	{
		SYSERROR("Could not shadow depth resource view");
		return false;
	}

	myDepth = depth;
	myDepthsResource = view;

	ID3D11Texture2D* tex2;
	CD3D11_TEXTURE2D_DESC desc2;
	WIPE(desc2);
	desc2.BindFlags = D3D11_BIND_RENDER_TARGET;
	desc2.Usage = D3D11_USAGE_DEFAULT;

	desc2.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc2.Height = SHADOWRESOLUTION;
	desc2.Width = SHADOWRESOLUTION * 6;
	desc2.ArraySize = 1;
	desc2.MipLevels = 1;
	desc2.SampleDesc.Count = 1;
	desc2.SampleDesc.Quality = 0;

	result = device->CreateTexture2D(&desc2, nullptr, &tex2);
	if (FAILED(result))
	{
		SYSERROR("Could not shadow depth target texture");
		return false;
	}

	result = device->CreateRenderTargetView(tex2, nullptr, &myRenderTarget);
	if (FAILED(result))
	{
		SYSERROR("Could not shadow depth target view");
		return false;
	}
	tex2->Release();


	D3D11_BUFFER_DESC bufferDescription;
	ZeroMemory(&bufferDescription, sizeof(bufferDescription));
	bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	const size_t buffercount = 2;

	ID3D11Buffer** buffers[buffercount] = {
		&myFrameBuffer,
		&myObjectBuffer};
	size_t sizes[buffercount] = {
		sizeof(FrameBufferData),
		sizeof(ObjectBufferData)
	};

	for (size_t i = 0; i < buffercount; i++)
	{
		bufferDescription.ByteWidth = CAST(UINT, sizes[i]);
		result = device->CreateBuffer(&bufferDescription, nullptr, buffers[i]);
		if (FAILED(result))
		{
			SYSCRASH("Could not create buffer");
			return false;
		}
	}

	return true;
}

const std::array<Camera*, 6>& ShadowRenderer::GetCameras()
{
	return myCameras;
}

void ShadowRenderer::BindshadowsToSlots(int aSlot)
{
	myContext->PSSetShaderResources(aSlot, 1, &myDepthsResource);
}

void ShadowRenderer::Render(PointLight* aLight, Scene* aScene, std::unordered_map<ModelInstance*, short>& aBoneMapping)
{
	
	auto preCull = aScene->Cull(CommonUtilities::Sphere<float>(aLight->position, SHADOWFARPLANE * 1.73f));


	myContext->PSSetShader(*myShader,nullptr,0);

	myContext->ClearDepthStencilView(myDepth, D3D11_CLEAR_DEPTH, 1.f, 0);
	for (size_t i = 0; i < 6; i++)
	{
		D3D11_VIEWPORT port;
		WIPE(port);
		port.Height = SHADOWRESOLUTION;
		port.Width = SHADOWRESOLUTION;
		port.MinDepth = 0.f;
		port.MaxDepth = 1.f;
		port.TopLeftX = SHADOWRESOLUTION*i;
		port.TopLeftY = 0;
		myContext->RSSetViewports(1, &port);
		myCameras[i]->SetPosition(aLight->position);
		std::vector<class ModelInstance*> filtered = aScene->Cull(myCameras[i],preCull);


		Model* model = nullptr;
		Model::CModelData* modelData = nullptr;


		HRESULT result;
		D3D11_MAPPED_SUBRESOURCE bufferData;

		WIPE(bufferData);
		result = myContext->Map(myFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);

		if (FAILED(result))
		{
			SYSERROR("Could not map frame buffer");
			return;
		}

		FrameBufferData fData;

		WIPE(fData);

		fData.myWorldToCamera = CommonUtilities::Matrix4x4<float>::Transpose(CommonUtilities::Matrix4x4<float>::GetFastInverse(myCameras[i]->GetTransform()));
		fData.myCameraToProjection = CommonUtilities::Matrix4x4<float>::Transpose(myCameras[i]->GetProjection());
		fData.myCameraPosition = myCameras[i]->GetPosition();
		fData.myTotalTime = Tools::GetTotalTime();


		memcpy(bufferData.pData, &fData, sizeof(fData));

		myContext->Unmap(myFrameBuffer, 0);

		myContext->VSSetConstantBuffers(0, 1, &myFrameBuffer);
		myContext->PSSetConstantBuffers(0, 1, &myFrameBuffer);
		myContext->GSSetShader(nullptr, nullptr, 0);

		ObjectBufferData oData;
		myContext->OMSetRenderTargets(1, &myRenderTarget, myDepth);
		for (size_t modelIndex = 0; modelIndex < filtered.size(); modelIndex++)
		{

			model = filtered[modelIndex]->GetModel();
			if (!model->ShouldRender())
			{
				continue;
			}
			modelData = model->GetModelData();

			oData.myModelToWorldSpace = CommonUtilities::Matrix4x4<float>::Transpose(filtered[modelIndex]->GetModelToWorldTransform());
			oData.myTint = filtered[modelIndex]->GetTint();
			if (modelData->myshaderTypeFlags & ShaderFlags::HasBones)
			{
				oData.myBoneOffsetIndex = aBoneMapping[filtered[modelIndex]];
			}
			if (modelData->myForceForward)
			{
				continue;
			}

			oData.myObjectLifeTime = Tools::GetTotalTime() - filtered[modelIndex]->GetSpawnTime();
			oData.myObjectExpectedLifeTime = filtered[modelIndex]->GetExpectedLifeTime();

			WIPE(bufferData);
			result = myContext->Map(myObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);

			if (FAILED(result))
			{
				SYSERROR("Could not map object buffer");
				return;
			}

			memcpy(bufferData.pData, &oData, sizeof(oData));
			myContext->Unmap(myObjectBuffer, 0);



			myContext->IASetPrimitiveTopology(modelData->myPrimitiveTopology);
			myContext->IASetInputLayout(modelData->myInputLayout);


			myContext->VSSetConstantBuffers(1, 1, &myObjectBuffer);
			myContext->VSSetShader(*modelData->myVertexShader, nullptr, 0);

			myContext->PSSetConstantBuffers(1, 1, &myObjectBuffer);
			myContext->PSSetShaderResources(0, 3, modelData->myTextures);
			myContext->VSSetShaderResources(0, 3, modelData->myTextures);

			Model::LodLevel* lodlevel = model->GetOptimalLodLevel(filtered[modelIndex]->GetPosition().Distance(myCameras[i]->GetPosition()));
			if (lodlevel)
			{
				myContext->IASetVertexBuffers(0, 1, &lodlevel->myVertexBuffer, &modelData->myStride, &modelData->myOffset);
				myContext->IASetIndexBuffer(lodlevel->myIndexBuffer, modelData->myIndexBufferFormat, 0);
				myContext->DrawIndexed(lodlevel->myNumberOfIndexes, 0, 0);
			}
			else
			{
				ONETIMEWARNING("Rendered without any loaded lod levels");
			}
		}
	}



}

