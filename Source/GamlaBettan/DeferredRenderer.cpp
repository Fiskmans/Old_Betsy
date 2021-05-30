#include <pch.h>
#include "DeferredRenderer.h"
#include <Matrix4x4.hpp>
#include <Vector4.hpp>
#include <d3d11.h>
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
#include <Intersection.hpp>
#include "ShaderBuffers.h"
#include "TimeHelper.h"
#include "FullscreenTexture.h"
#include "DirectX11Framework.h"
#include "DepthRenderer.h"
#include "Decal.h"
#include "RenderManager.h"
#include "FoldNumbers.h"
#include "AssetManager.h"

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

std::vector<class ModelInstance*> DeferredRenderer::GenerateGBuffer(Camera* aCamera, std::vector<class ModelInstance*>& aModelList, std::unordered_map<ModelInstance*, short>& aBoneMapping, FullscreenTexture* aBacksideTexture, RenderStateManager* aRenderStateManager, std::vector<class Decal*>& aDecals, GBuffer* aGBuffer, GBuffer* aBufferGBuffer, FullscreenRenderer& aFullscreenRenderer, Scene* aScene, FullscreenTexture* aDepth, BoneTextureCPUBuffer& aBoneTextureBuffer)
{

	std::vector<class ModelInstance*> filtered;
	std::vector<class ModelInstance*> drawn;
	Model* model = nullptr;
	Model::ModelData* modelData = nullptr;


	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE bufferData;

	WIPE(bufferData);

	FrameBufferData fData;
	WIPE(fData);

	fData.myWorldToCamera = CommonUtilities::Matrix4x4<float>::Transpose(CommonUtilities::Matrix4x4<float>::GetFastInverse(aCamera->GetTransform()));
	fData.myCameraPosition = aCamera->GetPosition();
	fData.myCameraDirection = aCamera->GetForward();
	fData.myTotalTime = RenderManager::GetTotalTime();

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
	ObjectBufferData oData;
	PERFORMANCETAG("standard");
	{

		for (size_t i = 0; i < aModelList.size(); i++)
		{
			model = aModelList[i]->GetModelAsset().GetAsModel();
			if (!model->ShouldRender())
			{
				continue;
			}
			modelData = model->GetModelData();
			myContext->PSSetShader(GetPixelShader(modelData->myshaderTypeFlags).GetAsPixelShader(), nullptr, 0);


			result = myContext->Map(myFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
			if (FAILED(result))
			{
				SYSERROR("Could not map frame buffer", "");
				return aModelList;
			}
			fData.myCameraToProjection = CommonUtilities::Matrix4x4<float>::Transpose(aCamera->GetProjection(aModelList[i]->GetIsUsingSecondaryFov()));
			memcpy(bufferData.pData, &fData, sizeof(fData));
			myContext->Unmap(myFrameBuffer, 0);

			oData.myModelToWorldSpace = CommonUtilities::Matrix4x4<float>::Transpose(aModelList[i]->GetModelToWorldTransformWithPotentialBoneAttachement(aBoneTextureBuffer, aBoneMapping));
			oData.myTint = aModelList[i]->GetTint();
			oData.myObjectId = Math::FoldPointer(aModelList[i]);
			if (modelData->myshaderTypeFlags & ShaderFlags::HasBones)
			{
				oData.myBoneOffsetIndex = aBoneMapping[aModelList[i]];
			}
			if (modelData->myForceForward || aModelList[i]->ShouldBeDrawnThroughWalls() || oData.myTint != V4F(0, 0, 0, 1))
			{
				filtered.push_back(aModelList[i]);
				continue;
			}
			drawn.push_back(aModelList[i]);
			oData.myIsEventActive = ModelInstance::GetEventStatus();

			oData.myObjectLifeTime = Tools::GetTotalTime() - aModelList[i]->GetSpawnTime();
			oData.myObjectExpectedLifeTime = aModelList[i]->GetExpectedLifeTime();
			float lastinteract = aModelList[i]->GetLastInteraction();
			if (CLOSEENUF(lastinteract, -1.f))
			{
				oData.myTimeSinceLastInteraction = -1.f;
			}
			else
			{
				oData.myTimeSinceLastInteraction = Tools::GetTotalTime() - lastinteract;
			}

			WIPE(bufferData);
			result = myContext->Map(myObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);

			if (FAILED(result))
			{
				SYSERROR("Could not map object buffer", "");
				return aModelList;
			}

			memcpy(bufferData.pData, &oData, sizeof(oData));
			myContext->Unmap(myObjectBuffer, 0);



			myContext->IASetPrimitiveTopology(modelData->myPrimitiveTopology);
			myContext->IASetInputLayout(modelData->myInputLayout);


			myContext->VSSetConstantBuffers(1, 1, &myObjectBuffer);
			myContext->VSSetShader(modelData->myVertexShader.GetAsVertexShader(), nullptr, 0);

			myContext->PSSetConstantBuffers(1, 1, &myObjectBuffer);

			ID3D11ShaderResourceView* resources[3] =
			{
				nullptr,
				nullptr,
				nullptr
			};
			if (modelData->myTextures[0].IsValid()) { resources[0] = modelData->myTextures[0].GetAsTexture(); }
			if (modelData->myTextures[1].IsValid()) { resources[1] = modelData->myTextures[1].GetAsTexture(); }
			if (modelData->myTextures[2].IsValid()) { resources[2] = modelData->myTextures[2].GetAsTexture(); }

			myContext->PSSetShaderResources(0, 3, resources);
			myContext->VSSetShaderResources(0, 3, resources);

			Model::LodLevel* lodlevel = model->GetOptimalLodLevel(aModelList[i]->GetPosition().DistanceSqr(aCamera->GetPosition()));
			if (lodlevel)
			{
				myContext->IASetVertexBuffers(0, lodlevel->myVertexBufferCount, lodlevel->myVertexBuffer, &modelData->myStride, &modelData->myOffset);
				myContext->IASetIndexBuffer(lodlevel->myIndexBuffer, modelData->myIndexBufferFormat, 0);
				myContext->DrawIndexed(lodlevel->myNumberOfIndexes, 0, 0);
			}
			else
			{
				ONETIMEWARNING("Rendered without any loaded lod levels", "");
			}
		}
	}

	if (aBacksideTexture)
	{
		PERFORMANCETAG("Backside");
		aBacksideTexture->SetAsActiveTarget();
		aRenderStateManager->SetRasterizerState(RenderStateManager::RasterizerState::CullFrontFacing);
		myContext->PSSetShader(myBackFaceShader.GetAsPixelShader(), nullptr, 0);


		for (size_t i = 0; i < drawn.size(); i++)
		{
			model = drawn[i]->GetModelAsset().GetAsModel();
			modelData = model->GetModelData();

			oData.myModelToWorldSpace = CommonUtilities::Matrix4x4<float>::Transpose(drawn[i]->GetModelToWorldTransform());
			if (modelData->myshaderTypeFlags & ShaderFlags::HasBones)
			{
				oData.myBoneOffsetIndex = aBoneMapping[drawn[i]];
			}

			oData.myObjectLifeTime = Tools::GetTotalTime() - drawn[i]->GetSpawnTime();
			oData.myObjectExpectedLifeTime = drawn[i]->GetExpectedLifeTime();

			WIPE(bufferData);
			result = myContext->Map(myObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);

			if (FAILED(result))
			{
				SYSERROR("Could not map object buffer", "");
				return aModelList;
			}

			memcpy(bufferData.pData, &oData, sizeof(oData));
			myContext->Unmap(myObjectBuffer, 0);


			myContext->IASetPrimitiveTopology(modelData->myPrimitiveTopology);
			myContext->IASetInputLayout(modelData->myInputLayout);


			myContext->VSSetConstantBuffers(1, 1, &myObjectBuffer);
			myContext->VSSetShader(modelData->myVertexShader.GetAsVertexShader(), nullptr, 0);

			myContext->PSSetConstantBuffers(1, 1, &myObjectBuffer);

			ID3D11ShaderResourceView* resources[3] =
			{
				nullptr,
				nullptr,
				nullptr
			};
			if (modelData->myTextures[0].IsValid()) { resources[0] = modelData->myTextures[0].GetAsTexture(); }
			if (modelData->myTextures[1].IsValid()) { resources[1] = modelData->myTextures[1].GetAsTexture(); }
			if (modelData->myTextures[2].IsValid()) { resources[2] = modelData->myTextures[2].GetAsTexture(); }

			myContext->PSSetShaderResources(0, 3, resources);
			myContext->VSSetShaderResources(0, 3, resources);


			Model::LodLevel* lodlevel = model->GetOptimalLodLevel(drawn[i]->GetPosition().DistanceSqr(aCamera->GetPosition()));
			if (lodlevel)
			{
				myContext->IASetVertexBuffers(0, lodlevel->myVertexBufferCount, lodlevel->myVertexBuffer, &modelData->myStride, &modelData->myOffset);
				myContext->IASetIndexBuffer(lodlevel->myIndexBuffer, modelData->myIndexBufferFormat, 0);
				myContext->DrawIndexed(lodlevel->myNumberOfIndexes, 0, 0);
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
				i->myDepth = myShadowRenderer->RenderDecalDepth(i, aScene, aBoneMapping);
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
		for (auto& i : aDecals)
		{
			DefDecalBuffer fData;

			fData.Position = i->myCamera->GetPosition();
			if (!CommonUtilities::IntersectionSpherePlaneVolume(CommonUtilities::Sphere<float>(i->myCamera->GetPosition(), i->myRange * CUBEHALFSIZETOENCAPSULATINGSPHERERADIUS), aCamera->GenerateFrustum()))
			{
				continue;
			}

			myContext->PSSetShaderResources(8, 1, &i->myDepth);
			myContext->VSSetShaderResources(8, 1, &i->myDepth);

			ID3D11ShaderResourceView* reources[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };

			for (size_t texIndex = 0; texIndex < i->myTextures.size(); texIndex++)
			{
				resource[texIndex] = i->myTextures[texIndex].GetAsTexture();
			}
			myContext->PSSetShaderResources(9, i->myTextures.size(), resource);

			fData.myToCamera = M44f::GetFastInverse(i->myCamera->GetTransform());
			fData.myToProj = i->myCamera->GetProjection(false);
			fData.myLifeTime = now - i->myTimestamp;
			fData.myCustomData = i->myCustomData;

			WIPE(bufferData);
			result = myContext->Map(myDecalBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);

			if (FAILED(result))
			{
				SYSERROR("Could not map object buffer", "");
			}

			memcpy(bufferData.pData, &fData, sizeof(fData));
			myContext->Unmap(myDecalBuffer, 0);
			myContext->PSSetConstantBuffers(0, 1, &myDecalBuffer);
			aRenderStateManager->SetBlendState(RenderStateManager::BlendState::AlphaBlend);
			aRenderStateManager->SetRasterizerState(RenderStateManager::RasterizerState::Default);
			aFullscreenRenderer.Render(i->myPixelShader);
		}
		myContext->PSSetShaderResources(0, 16, clearResource);
		myContext->OMSetRenderTargets(8, clearview, nullptr);
		aBufferGBuffer->CopyTo(aGBuffer, myContext);
	}
	aGBuffer->SetAsActiveTarget(aDepth);

	return filtered;
}

void DeferredRenderer::Render(FullscreenRenderer& aFullscreenRenderer, std::vector<PointLight*>& aPointLightList, std::vector<SpotLight*>& aSpotLightList, Scene* aScene, RenderStateManager* aRenderStateManager, std::unordered_map<ModelInstance*, short>& aBoneMapping)
{

	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE bufferData;
	WIPE(bufferData);
	CommonUtilities::PlaneVolume<float> frustum = aScene->GetMainCamera()->GenerateFrustum();

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
		myShadowRenderer->RenderEnvironmentDepth(aScene->GetEnvironmentLight(), aScene, aBoneMapping);
		myContext->OMSetRenderTargets(1, &oldView, oldDepth);
		myContext->RSSetViewports(oldPortCount, oldPort);
		myContext->PSSetShaderResources(0, 16, oldShaderResources);
		myShadowRenderer->BindshadowsToSlots(9);

		MapEnvLightBuffer(aScene);
		myContext->PSSetShaderResources(10, 1, perlinResource);
		aRenderStateManager->SetSamplerState(RenderStateManager::SamplerState::Trilinear);
		aFullscreenRenderer.Render(FullscreenRenderer::Shader::PBREnvironmentLight);
	}
	{
		PERFORMANCETAG("PointLights");
		DefPixelPointLightBuffer fData;

		WIPE(fData);
		fData.CameraPosition = aScene->GetMainCamera()->GetPosition();
		aRenderStateManager->SetBlendState(RenderStateManager::BlendState::AdditativeBlend);
		myContext->PSSetShaderResources(7, 1, perlinResource);
		const auto& shadowCameras = myShadowRenderer->GetCameras();

		for (auto& i : aPointLightList)
		{
			if (!CommonUtilities::IntersectionSpherePlaneVolume(CommonUtilities::Sphere<float>(i->position, i->range), frustum))
			{
				continue;
			}

			fData.Intensity = i->intensity;
			fData.LightColor = i->color;
			fData.Position = i->position;
			fData.Range = i->range;

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
				myShadowRenderer->Render(i, aScene, aBoneMapping);
				myContext->OMSetRenderTargets(1, &oldView, oldDepth);
				myContext->RSSetViewports(oldPortCount, oldPort);
				myContext->PSSetShaderResources(0, 16, oldShaderResources);
			}

			myShadowRenderer->BindshadowsToSlots(8);
			myContext->PSSetShaderResources(7, 1, perlinResource);
			//Render Shadows on/to resource 8-14

			for (size_t i = 0; i < 6; i++)
			{
				fData.myToCamera[i] = M44f::GetFastInverse(shadowCameras[i]->GetTransform());
				fData.myToProj[i] = shadowCameras[i]->GetProjection(false);
			}


			WIPE(bufferData);
			result = myContext->Map(myPixelPointLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);

			if (FAILED(result))
			{
				SYSERROR("Could not map object buffer", "");
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
				myShadowRenderer->RenderSpotLightDepth(i, aScene, aBoneMapping);
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
			fData.myToProj = i->myCamera->GetProjection(false);


			WIPE(bufferData);
			result = myContext->Map(myPixelSpotLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);

			if (FAILED(result))
			{
				SYSERROR("Could not map object buffer", "");
			}

			memcpy(bufferData.pData, &fData, sizeof(fData));
			myContext->Unmap(myPixelSpotLightBuffer, 0);
			myContext->PSSetConstantBuffers(0, 1, &myPixelSpotLightBuffer);
			aFullscreenRenderer.Render(FullscreenRenderer::Shader::PBRSpotLight);
		}
	}

}

void DeferredRenderer::MapEnvLightBuffer(Scene* aScene)
{

	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE bufferData;

	result = myContext->Map(myPixelEnvLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);

	if (FAILED(result))
	{
		SYSERROR("Could not map frame buffer", "");
	}

	DefPixelEnvLightBuffer fData;

	WIPE(fData);

	fData.myCameraPosition = aScene->GetMainCamera()->GetPosition();
	EnvironmentLight* envlight = aScene->GetEnvironmentLight();
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
	fData.myToProjection = cam->GetProjection(false);
	fData.time = Tools::GetTotalTime();
	fData.myCloudIntensity = myCloudIntensity;

	memcpy(bufferData.pData, &fData, sizeof(fData));

	myContext->Unmap(myPixelEnvLightBuffer, 0);
	myContext->PSSetConstantBuffers(0, 1, &myPixelEnvLightBuffer);
}

AssetHandle& DeferredRenderer::GetPixelShader(size_t flags)
{
	if (myPixelShaders.count(flags) == 0)
	{
		myPixelShaders[flags] = AssetManager::GetInstance().GetPixelShader("deferred/Deferred.hlsl");
	}
	return myPixelShaders[flags];
}
