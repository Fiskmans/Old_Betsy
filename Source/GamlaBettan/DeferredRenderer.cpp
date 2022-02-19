#include <pch.h>
#include "DeferredRenderer.h"
#include "Model.h"
#include "ModelInstance.h"
#include "ShaderCompiler.h"
#include "Camera.h"
#include "FullscreenRenderer.h"
#include "PointLight.h"
#include "PostMaster.hpp"
#include "Environmentlight.h"
#include "RenderStateManager.h"
#include "TextureLoader.h"
#include "CommonUtilities\Intersection.hpp"
#include "ShaderBuffers.h"
#include "TimeHelper.h"
#include "Texture.h"
#include "DirectX11Framework.h"
#include "DepthRenderer.h"
#include "Decal.h"
#include "RenderManager.h"
#include "FoldNumbers.h"
#include "AssetManager.h"
#include "GamlaBettan\RenderScene.h"

struct DefPixelEnvLightBuffer
{
	V3F myCameraPosition = { 0, 0, 0 };
	float myLightIntensity = 1;
	V3F myLightColor = { 1, 1, 1 };
	float time = 0;
	V3F myLightDirection = { 1, 0, 0 };
	float myCloudIntensity = 0;
	M44f myToCamera;
	M44f myToProjection;
};

struct DefPixelPointLightBuffer
{
	M44f myToCamera[6];
	M44f myToProj[6];
	V3F LightColor = { 1.f, 1.f, 1.f };
	float Intensity = 1.f;
	V3F Position = { 0.f, 0.f, 0.f };
	float Range = 0.f;
	V3F CameraPosition = { 0.f, 0.f, 0.f };
	float trash = 0.f;
};

struct DefPixelSpotLightBuffer
{
	M44f myToCamera;
	M44f myToProj;
	V3F Position = { 0.f, 0.f, 0.f };
	float Intensity = 1.f;
	V3F CameraPosition = { 0.f, 0.f, 0.f };
	float Range = 0.f;
};

struct DefDecalBuffer
{
	M44f myToCamera;
	M44f myToProj;
	V4F myCustomData = { 0.f,0.f,0.f,0.f };
	V3F Position = { 0.f, 0.f, 0.f };
	float Intensity = 1.f;
	V3F CameraPosition = { 0.f, 0.f, 0.f };
	float Range = 0.f;
	float myLifeTime = 0.f;
	V3F _;
};


bool DeferredRenderer::Init(DirectX11Framework* aFramework, AssetHandle aPerlinHandle, DepthRenderer* aShadowRenderer)
{
	myShadowRenderer = aShadowRenderer;
	myDevice = aFramework->GetDevice();

	if (!aFramework)
	{
		SYSCRASH("Deferred renderer was not given a framework to work on");
		return false;
	}

	myContext = aFramework->GetContext();
	if (!myContext)
	{
		SYSCRASH("Deferred renderer could not get context from framework");
		return false;
	}

	ID3D11Device* device = aFramework->GetDevice();
	if (!device)
	{
		SYSCRASH("Could not get device from framework");
		return false;
	}


	HRESULT result;

	D3D11_BUFFER_DESC bufferDescription;
	ZeroMemory(&bufferDescription, sizeof(bufferDescription));
	bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	const size_t buffercount = 6;

	ID3D11Buffer** buffers[buffercount] = {
		&myFrameBuffer,
		&myObjectBuffer,
		&myPixelEnvLightBuffer,
		&myPixelPointLightBuffer,
		&myPixelSpotLightBuffer ,
		&myDecalBuffer };
	size_t sizes[buffercount] = {
		sizeof(FrameBufferData),
		sizeof(ObjectBufferData),
		sizeof(DefPixelEnvLightBuffer),
		sizeof(DefPixelPointLightBuffer),
		sizeof(DefPixelSpotLightBuffer),
		sizeof(DefDecalBuffer)
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

	myBackFaceShader = AssetManager::GetInstance().GetPixelShader("deferred/Deferred_backfacing.hlsl");

	SYSINFO("Deferred renderer launched correctly");

	myPerlinHandle = aPerlinHandle;
	myCreateTime = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch().count();

	return true;
}

std::vector<ModelInstance*> DeferredRenderer::GenerateGBuffer(Camera* aCamera, std::vector<ModelInstance*>& aModelList, BoneTextureCPUBuffer& aBoneBuffer, std::unordered_map<ModelInstance*, short>& aBoneMapping, Texture* aBacksideTexture, RenderStateManager* aRenderStateManager, std::vector<Decal*>& aDecals, GBuffer* aGBuffer, GBuffer* aBufferGBuffer, FullscreenRenderer& aFullscreenRenderer, Texture* aDepth, BoneTextureCPUBuffer& aBoneTextureBuffer)
{

	std::vector<ModelInstance*> filtered;
	std::vector<ModelInstance*> drawn;

	{
		FrameBufferData fData;
		WIPE(fData);

		fData.myWorldToCamera = CommonUtilities::Matrix4x4<float>::Transpose(CommonUtilities::Matrix4x4<float>::GetFastInverse(aCamera->GetTransform()));
		fData.myCameraPosition = aCamera->GetPosition();
		fData.myCameraDirection = aCamera->GetForward();
		fData.myTotalTime = RenderManager::GetTotalTime();
		fData.myCameraToProjection = CommonUtilities::Matrix4x4<float>::Transpose(aCamera->GetProjection());

		if (!OverWriteBuffer(myFrameBuffer, &fData, sizeof(fData))) { return {}; }
	}

	ID3D11ShaderResourceView* resource[1] =
	{
		myPerlinHandle.GetAsTexture()
	};

	myContext->VSSetConstantBuffers(0, 1, &myFrameBuffer);
	myContext->PSSetConstantBuffers(0, 1, &myFrameBuffer);

	myContext->VSSetShaderResources(7, 1, resource);
	myContext->PSSetShaderResources(7, 1, resource);

	myContext->GSSetShader(nullptr, nullptr, 0);

	aGBuffer->SetAsActiveTarget(aDepth);
	{
		PERFORMANCETAG("standard");
		for (ModelInstance* modelInstance : aModelList)
		{
			Model* model = modelInstance->GetModelAsset().GetAsModel();
			if (!model->ShouldRender())
			{
				continue;
			}
			if (model->ShouldRenderWithForwardRenderer())
			{
				filtered.push_back(modelInstance);
				continue;
			}
			if (modelInstance->GetTint() != V4F(0, 0, 0, 1))
			{
				filtered.push_back(modelInstance);
				continue;
			}

			for (Model::ModelData* modelData : model->GetModelData())
			{
				ObjectBufferData oData;
				WIPE(oData);
				oData.myModelToWorldSpace =
					M44f::Transpose(modelInstance->GetModelToWorldTransformWithPotentialBoneAttachement(aBoneTextureBuffer, aBoneMapping))
					* modelData->myOffset;
				oData.myTint = V4F(0, 0, 0, 1);
				oData.myObjectId = Math::BinaryFold(modelInstance);
				oData.myDiffuseColor = modelData->myDiffuseColor;

				if (modelData->myshaderTypeFlags & ShaderFlags::HasBones)
				{
					oData.myBoneOffsetIndex = aBoneMapping[modelInstance];
				}
				else
				{
					oData.myBoneOffsetIndex = 0;
				}

				oData.myObjectLifeTime = Tools::GetTotalTime() - modelInstance->GetSpawnTime();


				if (!OverWriteBuffer(myObjectBuffer, &oData, sizeof(oData))) { return {}; }

				myContext->IASetPrimitiveTopology(modelData->myPrimitiveTopology);
				myContext->IASetInputLayout(modelData->myInputLayout);

				myContext->VSSetConstantBuffers(1, 1, &myObjectBuffer);
				myContext->PSSetConstantBuffers(1, 1, &myObjectBuffer);

				myContext->VSSetShader(modelData->myVertexShader.GetAsVertexShader(), nullptr, 0);
				myContext->PSSetShader(modelData->myPixelShader.GetAsPixelShader(), nullptr, 0);


				ID3D11ShaderResourceView* resources[3] = { nullptr };

				if (modelData->myTextures[0].IsValid()) { resources[0] = modelData->myTextures[0].GetAsTexture(); }
				if (modelData->myTextures[1].IsValid()) { resources[1] = modelData->myTextures[1].GetAsTexture(); }
				if (modelData->myTextures[2].IsValid()) { resources[2] = modelData->myTextures[2].GetAsTexture(); }

				myContext->PSSetShaderResources(0, 3, resources);
				myContext->VSSetShaderResources(0, 3, resources);

				UINT vertexOffset = 0;

				myContext->IASetVertexBuffers(0, 1, &modelData->myVertexBuffer, &modelData->myStride, &vertexOffset);

				if (modelData->myIsIndexed)
				{
					myContext->IASetIndexBuffer(modelData->myIndexBuffer, modelData->myIndexBufferFormat, 0);
					myContext->DrawIndexed(modelData->myNumberOfIndexes, 0, 0);
				}
				else
				{
					myContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_R8_TYPELESS, 0);
					myContext->Draw(modelData->myNumberOfVertexes, 0);
				}
			}
			drawn.push_back(modelInstance);
		}
	}

	if (aBacksideTexture)
	{
		PERFORMANCETAG("Backside");
		aBacksideTexture->SetAsActiveTarget();
		aRenderStateManager->SetRasterizerState(RenderStateManager::RasterizerState::CullFrontFacing);
		myContext->PSSetShader(myBackFaceShader.GetAsPixelShader(), nullptr, 0);


		for (ModelInstance* modelInstance : drawn)
		{
			Model* model = modelInstance->GetModelAsset().GetAsModel();
			for (Model::ModelData* modelData : model->GetModelData())
			{

				ObjectBufferData oData;
				oData.myModelToWorldSpace =
					M44f::Transpose(modelInstance->GetModelToWorldTransformWithPotentialBoneAttachement(aBoneTextureBuffer, aBoneMapping))
					* modelData->myOffset;
				if (modelData->myshaderTypeFlags & ShaderFlags::HasBones)
				{
					oData.myBoneOffsetIndex = aBoneMapping[modelInstance];
				}

				oData.myObjectLifeTime = Tools::GetTotalTime() - modelInstance->GetSpawnTime();

				if (!OverWriteBuffer(myObjectBuffer, &oData, sizeof(oData))) { return {}; }

				myContext->IASetPrimitiveTopology(modelData->myPrimitiveTopology);
				myContext->IASetInputLayout(modelData->myInputLayout);


				myContext->VSSetConstantBuffers(1, 1, &myObjectBuffer);
				myContext->VSSetShader(modelData->myVertexShader.GetAsVertexShader(), nullptr, 0);

				myContext->PSSetConstantBuffers(1, 1, &myObjectBuffer);

				ID3D11ShaderResourceView* resources[3] = { nullptr };
				if (modelData->myTextures[0].IsValid()) { resources[0] = modelData->myTextures[0].GetAsTexture(); }
				if (modelData->myTextures[1].IsValid()) { resources[1] = modelData->myTextures[1].GetAsTexture(); }
				if (modelData->myTextures[2].IsValid()) { resources[2] = modelData->myTextures[2].GetAsTexture(); }

				myContext->PSSetShaderResources(0, 3, resources);
				myContext->VSSetShaderResources(0, 3, resources);

				UINT vertexOffset = 0;

				myContext->IASetVertexBuffers(0, 1, &modelData->myVertexBuffer, &modelData->myStride, &vertexOffset);
				if (modelData->myIsIndexed)
				{
					myContext->IASetIndexBuffer(modelData->myIndexBuffer, modelData->myIndexBufferFormat, 0);
					myContext->DrawIndexed(modelData->myNumberOfIndexes, 0, 0);
				}
				else
				{
					myContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_R8_TYPELESS, 0);
					myContext->Draw(modelData->myNumberOfVertexes, 0);
				}
			}
		}
	}
	{
		PERFORMANCETAG("Decals");

		ID3D11RenderTargetView* oldView[8];
		ID3D11DepthStencilView* oldDepth;
		ID3D11ShaderResourceView* oldShaderResources[16];

		D3D11_VIEWPORT oldPort[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
		UINT oldPortCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;

		aRenderStateManager->SetRasterizerState(RenderStateManager::RasterizerState::Default);
		aRenderStateManager->SetSamplerState(RenderStateManager::SamplerState::Trilinear);
		aRenderStateManager->SetDepthStencilState(RenderStateManager::DepthStencilState::Default);
		for (auto& i : aDecals)
		{
			if (!i->myDepth)
			{
				PERFORMANCETAG("Decal Depth");
				myContext->PSGetShaderResources(0, 16, oldShaderResources);
				myContext->RSGetViewports(&oldPortCount, oldPort);
				myContext->OMGetRenderTargets(8, oldView, &oldDepth);
				i->myDepth = myShadowRenderer->RenderDecalDepth(i, aBoneBuffer, aBoneMapping);
				myContext->OMSetRenderTargets(8, oldView, oldDepth);
				myContext->RSSetViewports(oldPortCount, oldPort);
				myContext->PSSetShaderResources(0, 16, oldShaderResources);
				//LOGVERBOSE("Drew a decalDepth");
			}
		}
		ID3D11RenderTargetView* clearview[8] = { nullptr };
		ID3D11ShaderResourceView* clearResource[16] = {};

		myContext->OMSetRenderTargets(8, clearview, nullptr);
		myContext->PSSetShaderResources(0, 16, clearResource);
		aGBuffer->CopyTo(aBufferGBuffer, myContext);

		aGBuffer->SetAllAsResources();

		aBufferGBuffer->SetAsActiveTarget();
		float now = Tools::GetTotalTime();
		for (Decal* decal : aDecals)
		{
			DefDecalBuffer fData;

			fData.Position = decal->myCamera->GetPosition();
			if (!CommonUtilities::IntersectionSpherePlaneVolume(CommonUtilities::Sphere<float>(decal->myCamera->GetPosition(), decal->myRange * CUBEHALFSIZETOENCAPSULATINGSPHERERADIUS), aCamera->GenerateFrustum()))
			{
				continue;
			}

			myContext->PSSetShaderResources(8, 1, &decal->myDepth);
			myContext->VSSetShaderResources(8, 1, &decal->myDepth);

			ID3D11ShaderResourceView* reources[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };

			for (size_t texIndex = 0; texIndex < decal->myTextures.size(); texIndex++)
			{
				resource[texIndex] = decal->myTextures[texIndex].GetAsTexture();
			}
			myContext->PSSetShaderResources(9, static_cast<UINT>(decal->myTextures.size()), resource);

			fData.myToCamera = M44f::GetFastInverse(decal->myCamera->GetTransform());
			fData.myToProj = decal->myCamera->GetProjection();
			fData.myLifeTime = now - decal->myTimestamp;
			fData.myCustomData = decal->myCustomData;

			if (!OverWriteBuffer(myDecalBuffer, &fData, sizeof(fData))) { return {}; }

			myContext->PSSetConstantBuffers(0, 1, &myDecalBuffer);
			aRenderStateManager->SetBlendState(RenderStateManager::BlendState::AlphaBlend);
			aRenderStateManager->SetRasterizerState(RenderStateManager::RasterizerState::Default);
			aFullscreenRenderer.Render(decal->myPixelShader);
		}
		myContext->PSSetShaderResources(0, 16, clearResource);
		myContext->OMSetRenderTargets(8, clearview, nullptr);
		aBufferGBuffer->CopyTo(aGBuffer, myContext);
	}
	aGBuffer->SetAsActiveTarget(aDepth);

	return filtered;
}

void DeferredRenderer::Render(FullscreenRenderer& aFullscreenRenderer, std::vector<PointLight*>& aPointLightList, std::vector<SpotLight*>& aSpotLightList, RenderStateManager* aRenderStateManager, BoneTextureCPUBuffer& aBoneBuffer, std::unordered_map<ModelInstance*, short>& aBoneMapping)
{

	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE bufferData;
	WIPE(bufferData);
	CommonUtilities::PlaneVolume<float> frustum = RenderScene::GetInstance().GetMainCamera()->GenerateFrustum();

	ID3D11ShaderResourceView* perlinResource[1] =
	{
		myPerlinHandle.GetAsTexture()
	};

	{
		PERFORMANCETAG("Environmentlight");

		ID3D11RenderTargetView* oldView;
		ID3D11DepthStencilView* oldDepth;
		ID3D11ShaderResourceView* oldShaderResources[16];

		D3D11_VIEWPORT oldPort[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
		UINT oldPortCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;

		myContext->PSGetShaderResources(0, 16, oldShaderResources);
		myContext->RSGetViewports(&oldPortCount, oldPort);
		myContext->OMGetRenderTargets(1, &oldView, &oldDepth);
		myShadowRenderer->RenderEnvironmentDepth(RenderScene::GetInstance().GetEnvironmentLight(), aBoneBuffer, aBoneMapping);
		myContext->OMSetRenderTargets(1, &oldView, oldDepth);
		myContext->RSSetViewports(oldPortCount, oldPort);
		myContext->PSSetShaderResources(0, 16, oldShaderResources);
		myShadowRenderer->BindshadowsToSlots(9);

		MapEnvLightBuffer();
		myContext->PSSetShaderResources(10, 1, perlinResource);
		aRenderStateManager->SetSamplerState(RenderStateManager::SamplerState::Trilinear);
		aFullscreenRenderer.Render(FullscreenRenderer::Shader::PBREnvironmentLight);
	}
	{
		PERFORMANCETAG("PointLights");
		DefPixelPointLightBuffer fData;

		WIPE(fData);
		fData.CameraPosition = RenderScene::GetInstance().GetMainCamera()->GetPosition();
		aRenderStateManager->SetBlendState(RenderStateManager::BlendState::AdditativeBlend);
		myContext->PSSetShaderResources(7, 1, perlinResource);
		const auto& shadowCameras = myShadowRenderer->GetCameras();

		for (auto& pintlight : aPointLightList)
		{
			if (!CommonUtilities::IntersectionSpherePlaneVolume(CommonUtilities::Sphere<float>(pintlight->position, pintlight->range), frustum))
			{
				continue;
			}

			fData.Intensity = pintlight->intensity;
			fData.LightColor = pintlight->color;
			fData.Position = pintlight->position;
			fData.Range = pintlight->range;

			{
				PERFORMANCETAG("Shadowmapping");
				ID3D11RenderTargetView* oldView;
				ID3D11DepthStencilView* oldDepth;
				ID3D11ShaderResourceView* oldShaderResources[16];

				D3D11_VIEWPORT oldPort[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
				UINT oldPortCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;

				myContext->PSGetShaderResources(0, 16, oldShaderResources);
				myContext->RSGetViewports(&oldPortCount, oldPort);
				myContext->OMGetRenderTargets(1, &oldView, &oldDepth);
				myShadowRenderer->Render(pintlight, aBoneBuffer, aBoneMapping);
				myContext->OMSetRenderTargets(1, &oldView, oldDepth);
				myContext->RSSetViewports(oldPortCount, oldPort);
				myContext->PSSetShaderResources(0, 16, oldShaderResources);
			}

			myShadowRenderer->BindshadowsToSlots(8);
			myContext->PSSetShaderResources(7, 1, perlinResource);
			//Render Shadows on/to resource 8-14

			for (size_t side = 0; side < 6; side++)
			{
				fData.myToCamera[side] = M44f::GetFastInverse(shadowCameras[side]->GetTransform());
				fData.myToProj[side] = shadowCameras[side]->GetProjection();
			}


			WIPE(bufferData);
			result = myContext->Map(myPixelPointLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);

			if (FAILED(result))
			{
				SYSERROR("Could not map object buffer");
			}

			memcpy(bufferData.pData, &fData, sizeof(fData));
			myContext->Unmap(myPixelPointLightBuffer, 0);
			myContext->PSSetConstantBuffers(0, 1, &myPixelPointLightBuffer);
			//aRenderStateManager->SetSamplerState(RenderStateManager::SamplerState::Trilinear);
			aFullscreenRenderer.Render(FullscreenRenderer::Shader::PBRPointLight);
		}
	}


	{
		PERFORMANCETAG("Spotlights");

		aRenderStateManager->SetSamplerState(RenderStateManager::SamplerState::Trilinear);
		for (auto& i : aSpotLightList)
		{
			DefPixelSpotLightBuffer fData;

			fData.Intensity = i->myIntensity;
			fData.Position = i->myCamera->GetPosition();
			fData.Range = i->myRange;
			if (!CommonUtilities::IntersectionSpherePlaneVolume(CommonUtilities::Sphere<float>(i->myCamera->GetPosition(), i->myRange * CUBEHALFSIZETOENCAPSULATINGSPHERERADIUS), frustum))
			{
				continue;
			}

			{
				PERFORMANCETAG("Shadowmapping");
				ID3D11RenderTargetView* oldView;
				ID3D11DepthStencilView* oldDepth;
				ID3D11ShaderResourceView* oldShaderResources[16];
				ID3D11ShaderResourceView* clearResources[16] = { nullptr };

				D3D11_VIEWPORT oldPort[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
				UINT oldPortCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;

				myContext->PSGetShaderResources(0, 16, oldShaderResources);
				myContext->RSGetViewports(&oldPortCount, oldPort);
				myContext->OMGetRenderTargets(1, &oldView, &oldDepth);
				myShadowRenderer->RenderSpotLightDepth(i, aBoneBuffer, aBoneMapping);
				myContext->PSSetShaderResources(0, 16, clearResources);
				myContext->OMSetRenderTargets(1, &oldView, oldDepth);
				myContext->RSSetViewports(oldPortCount, oldPort);
				myContext->PSSetShaderResources(0, 16, oldShaderResources);
			}

			myShadowRenderer->BindshadowsToSlots(8);
			ID3D11ShaderResourceView* resource[1] =
			{
				i->myTexture.GetAsTexture()
			};

			myContext->PSSetShaderResources(9, 1, resource);
			myContext->PSSetShaderResources(7, 1, perlinResource);
			//Render Shadows on/to resource 8-14

			fData.myToCamera = M44f::GetFastInverse(i->myCamera->GetTransform());
			fData.myToProj = i->myCamera->GetProjection();


			WIPE(bufferData);
			result = myContext->Map(myPixelSpotLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);

			if (FAILED(result))
			{
				SYSERROR("Could not map object buffer");
			}

			memcpy(bufferData.pData, &fData, sizeof(fData));
			myContext->Unmap(myPixelSpotLightBuffer, 0);
			myContext->PSSetConstantBuffers(0, 1, &myPixelSpotLightBuffer);
			aFullscreenRenderer.Render(FullscreenRenderer::Shader::PBRSpotLight);
		}
	}

}

void DeferredRenderer::MapEnvLightBuffer()
{

	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE bufferData;

	result = myContext->Map(myPixelEnvLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);

	if (FAILED(result))
	{
		SYSERROR("Could not map frame buffer");
	}

	DefPixelEnvLightBuffer fData;

	WIPE(fData);

	fData.myCameraPosition = RenderScene::GetInstance().GetMainCamera()->GetPosition();
	EnvironmentLight* envlight = RenderScene::GetInstance().GetEnvironmentLight();
	if (envlight)
	{
		ID3D11ShaderResourceView* texture[1] =
		{
			envlight->myTexture.GetAsTexture()
		};

		myContext->PSSetShaderResources(8, 1, texture);

		fData.myLightColor = envlight->myColor;
		fData.myLightDirection = envlight->myDirection;
		fData.myLightIntensity = envlight->myIntensity;
	}

	const Camera* cam = myShadowRenderer->GetEnvirontmentCamera();
	fData.myToCamera = M44f::GetFastInverse(cam->GetTransform());
	fData.myToProjection = cam->GetProjection();
	fData.time = Tools::GetTotalTime();
	fData.myCloudIntensity = myCloudIntensity;

	memcpy(bufferData.pData, &fData, sizeof(fData));

	myContext->Unmap(myPixelEnvLightBuffer, 0);
	myContext->PSSetConstantBuffers(0, 1, &myPixelEnvLightBuffer);
}

bool DeferredRenderer::OverWriteBuffer(ID3D11Buffer* aBuffer, void* aData, size_t aSize)
{
	D3D11_MAPPED_SUBRESOURCE bufferData;
	WIPE(bufferData);

	HRESULT result = myContext->Map(aBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);

	if (FAILED(result))
	{
		SYSERROR("Could not map buffer");
		return false;
	}

	memcpy(bufferData.pData, aData, aSize);
	myContext->Unmap(aBuffer, 0);

	return true;
}