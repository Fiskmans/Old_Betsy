#include "engine/graphics/DeferredRenderer.h"
#include "engine/graphics/ShaderBuffers.h"
#include "engine/graphics/GraphicEngine.h"
#include "engine/graphics/RenderManager.h"
#include "engine/graphics/ShaderMappings.h"
#include "engine/graphics/RenderScene.h"

#include "engine/assets/AssetManager.h"
#include "engine/assets/Model.h"

#include "tools/TimeHelper.h"
#include "tools/Functors.h"

namespace engine::graphics
{
	struct DeferredPixelEnvLightBuffer
	{
		tools::V3f myCameraPosition;
		float myLightIntensity;

		tools::V3f myLightColor;
		float time;

		tools::V3f myLightDirection;
		float myCloudIntensity;

		tools::M44f myToCamera;
		tools::M44f myToProjection;
	};

	struct DeferredPixelPointLightBuffer
	{
		tools::M44f myToCamera[6];
		tools::M44f myToProj[6];

		tools::V3f LightColor;
		float Intensity = 1.f;

		tools::V3f Position;
		float Range;

		tools::V3f CameraPosition;

		float padding;
	};

	struct DeferredPixelSpotLightBuffer
	{
		tools::M44f myToCamera;
		tools::M44f myToProj;
		tools::V3f Position;
		float Intensity;
		tools::V3f CameraPosition;
		float Range;
	};

	struct DeferredDecalBuffer
	{
		tools::M44f myToCamera;
		tools::M44f myToProj;
		tools::V4f myCustomData;

		tools::V3f Position;
		float Intensity;

		tools::V3f CameraPosition;
		float Range;

		float myLifeTime;

		tools::V3f padding;
	};


	bool DeferredRenderer::Init()
	{
		struct BufferAndSize
		{
			ID3D11Buffer** myBuffer;
			UINT mySize;
		};

		if (!RenderManager::CreateGenericShaderBuffer< FrameBuffer>(myFrameBuffer)) { return false; }
		if (!RenderManager::CreateGenericShaderBuffer< ObjectBuffer>(myObjectBuffer)) { return false; }
		if (!RenderManager::CreateGenericShaderBuffer< DeferredPixelEnvLightBuffer>(myPixelEnvLightBuffer)) { return false; }
		if (!RenderManager::CreateGenericShaderBuffer< DeferredPixelPointLightBuffer>(myPixelPointLightBuffer)) { return false; }
		if (!RenderManager::CreateGenericShaderBuffer< DeferredPixelSpotLightBuffer>(myPixelSpotLightBuffer)) { return false; }
		if (!RenderManager::CreateGenericShaderBuffer< DeferredDecalBuffer>(myDecalBuffer)) { return false; }

		myBackFaceShader = AssetManager::GetInstance().GetPixelShader("deferred/Deferred_backfacing.hlsl");

		if (!myBackFaceShader.IsValid())
		{
			LOG_SYS_CRASH("Failed to load required engine shader");
			return false;
		}

		LOG_INFO("Deferred Renderer initialization complete");

		return true;
	}

	DeferredRenderer::FilterResult DeferredRenderer::Filter(const std::vector<ModelInstance*>& aModels)
	{
		FilterResult out;

		for (ModelInstance* modelInstance : aModels)
		{
			if (!modelInstance->ShouldRender())
			{
				continue;
			}
			if (modelInstance->GetModelAsset().myModel->ShouldRenderWithForwardRenderer())
			{
				out.myRest.push_back(modelInstance);
				continue;
			}
			out.myDeferrable.push_back(modelInstance);
		}

		return FilterResult();
	}

	void DeferredRenderer::GenerateGBuffer(Camera* aCamera)
	{
		ID3D11DeviceContext* context = GraphicsEngine::GetInstance().GetFrameWork().GetContext();

		{
			FrameBuffer fData;
			WIPE(fData);

			fData.myWorldToCamera = aCamera->GetTransform().FastInverse();

			fData.myTotalTime = tools::GetTotalTime();
			fData.myCameraToProjection = aCamera->GetProjection();

			if (!RenderManager::OverWriteBuffer(myFrameBuffer, &fData, sizeof(fData)))
				return;
		}

		context->VSSetConstantBuffers(graphics::FRAME_BUFFER_INDEX, 1, &myFrameBuffer);
		context->PSSetConstantBuffers(graphics::FRAME_BUFFER_INDEX, 1, &myFrameBuffer);

		context->GSSetShader(nullptr, nullptr, 0);

		{
			PERFORMANCETAG("standard");
			for (ModelInstance* modelInstance : aCamera->Cull())
			{
				Model* model = modelInstance->GetModelAsset().myModel;

				for (Model::ModelData* modelData : model->GetModelData())
				{
					ObjectBuffer oData;
					WIPE(oData);
					oData.myModelToWorldSpace = modelInstance->GetModelToWorldTransform().Transposed() * modelData->myOffset;
					oData.myDiffuseColor = modelData->myDiffuseColor;

					oData.myObjectLifeTime = tools::GetTotalTime() - modelInstance->GetSpawnTime();
					oData.myObjectId = modelInstance->GetId();

					if (!RenderManager::OverWriteBuffer(myObjectBuffer, &oData, sizeof(oData))) { return; }

					if (modelInstance->HasAnimations())
						modelInstance->SetupanimationMatrixes();

					context->IASetPrimitiveTopology(modelData->myPrimitiveTopology);
					context->IASetInputLayout(modelData->myInputLayout);

					context->VSSetConstantBuffers(graphics::OBJECT_BUFFER_INDEX, 1, &myObjectBuffer);
					context->PSSetConstantBuffers(graphics::OBJECT_BUFFER_INDEX, 1, &myObjectBuffer);

					context->VSSetShader(modelData->myVertexShader.Get<VertexShaderAsset>().myShader, nullptr, 0);
					context->PSSetShader(modelData->myPixelShader.Get<PixelShaderAsset>().myShader, nullptr, 0);


					ID3D11ShaderResourceView* resources[3] = { nullptr };

					if (modelData->myTextures[0].IsValid()) { resources[0] = modelData->myTextures[0].Get<TextureAsset>().myTexture; }
					if (modelData->myTextures[1].IsValid()) { resources[1] = modelData->myTextures[1].Get<TextureAsset>().myTexture; }
					if (modelData->myTextures[2].IsValid()) { resources[2] = modelData->myTextures[2].Get<TextureAsset>().myTexture; }

					context->PSSetShaderResources(0, 3, resources);
					context->VSSetShaderResources(0, 3, resources);

					UINT vertexOffset = 0;

					context->IASetVertexBuffers(0, 1, &modelData->myVertexBuffer, &modelData->myStride, &vertexOffset);

					if (modelData->myIsIndexed)
					{
						context->IASetIndexBuffer(modelData->myIndexBuffer, modelData->myIndexBufferFormat, 0);
						context->DrawIndexed(modelData->myNumberOfIndexes, 0, 0);
					}
					else
					{
						context->IASetIndexBuffer(nullptr, DXGI_FORMAT_R8_TYPELESS, 0);
						context->Draw(modelData->myNumberOfVertexes, 0);
					}
				}
			}
		}
		/*

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
				DeferredDecalBuffer fData;

				fData.Position = decal->myCamera->GetPosition();
				if (!tools::IntersectionSpherePlaneVolume(tools::Sphere<float>(decal->myCamera->GetPosition(), decal->myRange * CUBEHALFSIZETOENCAPSULATINGSPHERERADIUS), aCamera->GenerateFrustum()))
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

				fData.myToCamera = tools::M44f::GetFastInverse(decal->myCamera->GetTransform());
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

		return filtered;*/
	}

	void DeferredRenderer::Render(Camera* aCamera)
	{

		//ID3D11DeviceContext* context = GraphicsEngine::GetInstance().GetFrameWork().GetContext();
		{
			PERFORMANCETAG("Environmentlight");

			//ID3D11RenderTargetView* oldView;
			//ID3D11DepthStencilView* oldDepth;
			//ID3D11ShaderResourceView* oldShaderResources[16];
			//
			//D3D11_VIEWPORT oldPort[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
			//UINT oldPortCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;

			//context->PSGetShaderResources(0, 16, oldShaderResources);
			//context->RSGetViewports(&oldPortCount, oldPort);
			//context->OMGetRenderTargets(1, &oldView, &oldDepth);
			//myShadowRenderer->RenderEnvironmentDepth(RenderScene::GetInstance().GetEnvironmentLight(), aBoneBuffer, aBoneMapping);
			//context->OMSetRenderTargets(1, &oldView, oldDepth);
			//context->RSSetViewports(oldPortCount, oldPort);
			//context->PSSetShaderResources(0, 16, oldShaderResources);
			//myShadowRenderer->BindshadowsToSlots(9);

			MapEnvLightBuffer(aCamera);
			RenderManager::GetInstance().GetRenderStateManager().SetSamplerState(RenderStateManager::SamplerState::Trilinear);
			RenderManager::GetInstance().GetFullscreenRender().Render(FullscreenRenderer::Shader::PBREnvironmentLight);
		}
		//{
		//	PERFORMANCETAG("PointLights");
		//	DefPixelPointLightBuffer fData;
		//
		//	WIPE(fData);
		//	fData.CameraPosition = RenderScene::GetInstance().GetMainCamera()->GetPosition();
		//	aRenderStateManager->SetBlendState(RenderStateManager::BlendState::AdditativeBlend);
		//	myContext->PSSetShaderResources(7, 1, perlinResource);
		//	const auto& shadowCameras = myShadowRenderer->GetCameras();
		//
		//	for (auto& pintlight : aPointLightList)
		//	{
		//		if (!CommonUtilities::IntersectionSpherePlaneVolume(CommonUtilities::Sphere<float>(pintlight->position, pintlight->range), frustum))
		//		{
		//			continue;
		//		}
		//
		//		fData.Intensity = pintlight->intensity;
		//		fData.LightColor = pintlight->color;
		//		fData.Position = pintlight->position;
		//		fData.Range = pintlight->range;
		//
		//		{
		//			PERFORMANCETAG("Shadowmapping");
		//			ID3D11RenderTargetView* oldView;
		//			ID3D11DepthStencilView* oldDepth;
		//			ID3D11ShaderResourceView* oldShaderResources[16];
		//
		//			D3D11_VIEWPORT oldPort[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
		//			UINT oldPortCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
		//
		//			myContext->PSGetShaderResources(0, 16, oldShaderResources);
		//			myContext->RSGetViewports(&oldPortCount, oldPort);
		//			myContext->OMGetRenderTargets(1, &oldView, &oldDepth);
		//			myShadowRenderer->Render(pintlight, aBoneBuffer, aBoneMapping);
		//			myContext->OMSetRenderTargets(1, &oldView, oldDepth);
		//			myContext->RSSetViewports(oldPortCount, oldPort);
		//			myContext->PSSetShaderResources(0, 16, oldShaderResources);
		//		}
		//
		//		myShadowRenderer->BindshadowsToSlots(8);
		//		myContext->PSSetShaderResources(7, 1, perlinResource);
		//		//Render Shadows on/to resource 8-14
		//
		//		for (size_t side = 0; side < 6; side++)
		//		{
		//			fData.myToCamera[side] = tools::M44f::GetFastInverse(shadowCameras[side]->GetTransform());
		//			fData.myToProj[side] = shadowCameras[side]->GetProjection();
		//		}
		//
		//
		//		WIPE(bufferData);
		//		result = myContext->Map(myPixelPointLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
		//
		//		if (FAILED(result))
		//		{
		//			SYSERROR("Could not map object buffer");
		//		}
		//
		//		memcpy(bufferData.pData, &fData, sizeof(fData));
		//		myContext->Unmap(myPixelPointLightBuffer, 0);
		//		myContext->PSSetConstantBuffers(0, 1, &myPixelPointLightBuffer);
		//		//aRenderStateManager->SetSamplerState(RenderStateManager::SamplerState::Trilinear);
		//		aFullscreenRenderer.Render(FullscreenRenderer::Shader::PBRPointLight);
		//	}
		//}


		//{
		//	PERFORMANCETAG("Spotlights");
		//
		//	aRenderStateManager->SetSamplerState(RenderStateManager::SamplerState::Trilinear);
		//	for (auto& i : aSpotLightList)
		//	{
		//		DefPixelSpotLightBuffer fData;
		//
		//		fData.Intensity = i->myIntensity;
		//		fData.Position = i->myCamera->GetPosition();
		//		fData.Range = i->myRange;
		//		if (!CommonUtilities::IntersectionSpherePlaneVolume(CommonUtilities::Sphere<float>(i->myCamera->GetPosition(), i->myRange * CUBEHALFSIZETOENCAPSULATINGSPHERERADIUS), frustum))
		//		{
		//			continue;
		//		}
		//
		//		{
		//			PERFORMANCETAG("Shadowmapping");
		//			ID3D11RenderTargetView* oldView;
		//			ID3D11DepthStencilView* oldDepth;
		//			ID3D11ShaderResourceView* oldShaderResources[16];
		//			ID3D11ShaderResourceView* clearResources[16] = { nullptr };
		//
		//			D3D11_VIEWPORT oldPort[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
		//			UINT oldPortCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
		//
		//			myContext->PSGetShaderResources(0, 16, oldShaderResources);
		//			myContext->RSGetViewports(&oldPortCount, oldPort);
		//			myContext->OMGetRenderTargets(1, &oldView, &oldDepth);
		//			myShadowRenderer->RenderSpotLightDepth(i, aBoneBuffer, aBoneMapping);
		//			myContext->PSSetShaderResources(0, 16, clearResources);
		//			myContext->OMSetRenderTargets(1, &oldView, oldDepth);
		//			myContext->RSSetViewports(oldPortCount, oldPort);
		//			myContext->PSSetShaderResources(0, 16, oldShaderResources);
		//		}
		//
		//		myShadowRenderer->BindshadowsToSlots(8);
		//		ID3D11ShaderResourceView* resource[1] =
		//		{
		//			i->myTexture.GetAsTexture()
		//		};
		//
		//		myContext->PSSetShaderResources(9, 1, resource);
		//		myContext->PSSetShaderResources(7, 1, perlinResource);
		//		//Render Shadows on/to resource 8-14
		//
		//		fData.myToCamera = tools::M44f::GetFastInverse(i->myCamera->GetTransform());
		//		fData.myToProj = i->myCamera->GetProjection();
		//
		//
		//		WIPE(bufferData);
		//		result = myContext->Map(myPixelSpotLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
		//
		//		if (FAILED(result))
		//		{
		//			SYSERROR("Could not map object buffer");
		//		}
		//
		//		memcpy(bufferData.pData, &fData, sizeof(fData));
		//		myContext->Unmap(myPixelSpotLightBuffer, 0);
		//		myContext->PSSetConstantBuffers(0, 1, &myPixelSpotLightBuffer);
		//		aFullscreenRenderer.Render(FullscreenRenderer::Shader::PBRSpotLight);
		//	}
		//}
	}

	void DeferredRenderer::MapEnvLightBuffer(Camera* aCamera)
	{
		ID3D11DeviceContext* context = GraphicsEngine::GetInstance().GetFrameWork().GetContext();
		HRESULT result;
		D3D11_MAPPED_SUBRESOURCE bufferData;

		result = context->Map(myPixelEnvLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);

		if (FAILED(result))
		{
			LOG_ERROR("Could not map frame buffer");
			return;
		}

		DeferredPixelEnvLightBuffer fData;

		WIPE(fData);

		fData.myCameraPosition = aCamera->GetPosition();
		EnvironmentLight* envlight = aCamera->GetScene().GetEnvironmentLight();
		if (envlight)
		{
			ID3D11ShaderResourceView* texture[1] { nullptr };

			if (envlight->myTexture.IsValid()) 
			{ 
				texture[0] = envlight->myTexture.Get<TextureAsset>().myTexture;
			}
			else
			{
				EXECUTE_ONCE({ LOG_WARNING("Environemnt light has no texture"); });
			}
		
			context->PSSetShaderResources(shader_mappings::TEXTURE_SKYBOX, 1, texture);
		
			fData.myLightColor = envlight->myColor;
			fData.myLightDirection = envlight->myDirection;
			fData.myLightIntensity = envlight->myIntensity;
		}

		//const Camera* cam = myShadowRenderer->GetEnvirontmentCamera();
		//fData.myToCamera = cam->GetTransform().FastInverse();
		//fData.myToProjection = cam->GetProjection();
		fData.time = tools::GetTotalTime();
		//fData.myCloudIntensity = myCloudIntensity;

		memcpy(bufferData.pData, &fData, sizeof(fData));

		context->Unmap(myPixelEnvLightBuffer, 0);
		context->PSSetConstantBuffers(shader_mappings::CONSTANT_BUFFER_ENV_LIGHT, 1, &myPixelEnvLightBuffer);
	}

}