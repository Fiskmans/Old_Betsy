#include "pch.h"
#include "ForwardRenderer.h"
#include "DirectX11Framework.h"
#include "Model.h"
#include "ModelInstance.h"
#include <d3d11.h>
#include <algorithm>
#include <functional>

#include "Camera.h"
#include "Scene.h"
#include "Environmentlight.h"
#include "PointLight.h"
#include "PostMaster.hpp"
#include "TextureLoader.h"
#include "ShaderBuffers.h"
#include "ShaderFlags.h"
#include "ShaderCompiler.h"
#include "RenderStateManager.h"
#include "TimeHelper.h"
#include "DepthRenderer.h"
#include "RenderManager.h"
#include "FoldNumbers.h"
#include "AssetManager.h"


ForwardRenderer::~ForwardRenderer()
{
	SAFE_RELEASE(myFrameBuffer);
	SAFE_RELEASE(myObjectBuffer);
}

bool ForwardRenderer::Init(DirectX11Framework* aFramework, const std::string& aThroughWallPSName, const std::string& aEnemyThroughWallPSName, AssetHandle aPerlinHandle, DepthRenderer* aDepthRenderer)
{
	if (!aFramework)
	{
		SYSCRASH("Forward renderer was not given a framework to work on");
		return false;
	}

	myContext = aFramework->GetContext();
	if (!myContext)
	{
		SYSCRASH("Forward renderer could not get context from framework");
		return false;
	}

	myDevice = aFramework->GetDevice();
	if (!myDevice)
	{
		SYSCRASH("Could not get myDevice from framework");
		return false;
	}


	HRESULT result;

	D3D11_BUFFER_DESC bufferDescription;
	ZeroMemory(&bufferDescription, sizeof(bufferDescription));
	bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	bufferDescription.ByteWidth = sizeof(FrameBufferData);
	result = myDevice->CreateBuffer(&bufferDescription, nullptr, &myFrameBuffer);
	if (FAILED(result))
	{
		SYSCRASH("Could not create frame data buffer");
		return false;
	}


	bufferDescription.ByteWidth = sizeof(ObjectBufferData);
	result = myDevice->CreateBuffer(&bufferDescription, nullptr, &myObjectBuffer);
	if (FAILED(result))
	{
		SYSCRASH("Could not create object data buffer");
		return false;
	}

	myThroughWallShader = AssetManager::GetInstance().GetPixelShader(aThroughWallPSName);
	myEnemyThroughWallShader = AssetManager::GetInstance().GetPixelShader(aEnemyThroughWallPSName);

	myPerlinHandle = aPerlinHandle;
	myDepthRender = aDepthRenderer;

	SYSINFO("Forward renderer launched correctly");

	return true;
}

typedef std::pair<ModelInstance*, std::array<PointLight*, NUMBEROFPOINTLIGHTS>*> ModelAndLights;

namespace RenderSort
{
	class FurthestFromCam
	{
	public:
		FurthestFromCam(Camera* aCamera) : myCamera(aCamera) {};

		bool operator()(const ModelAndLights& a, const ModelAndLights& b) const
		{
			return (a.first->GetPosition().DistanceSqr(myCamera->GetPosition()) > b.first->GetPosition().DistanceSqr(myCamera->GetPosition()));
		}

	private:
		Camera* myCamera;
	};

}

void ForwardRenderer::Render(std::vector<ModelInstance*>& aModelList, Camera* aCamera, std::vector<std::array<PointLight*, NUMBEROFPOINTLIGHTS>>& aLightList, std::unordered_map<ModelInstance*, short>& aBoneMapping, RenderStateManager& aStateManager, BoneTextureCPUBuffer& aBoneBuffer)
{
	std::vector<ModelAndLights> modelsAndLightsList;
	modelsAndLightsList.reserve(aModelList.size());
	aStateManager.SetSamplerState(RenderStateManager::SamplerState::Point);

	for (size_t i = 0; i < aModelList.size(); ++i)
	{
		modelsAndLightsList.push_back(ModelAndLights(aModelList[i], &aLightList[i]));
	}

	std::sort(modelsAndLightsList.begin(), modelsAndLightsList.end(), RenderSort::FurthestFromCam(aCamera));

	if (myskybox)
	{
		modelsAndLightsList.insert(modelsAndLightsList.begin(), ModelAndLights(myskybox, { nullptr }));
	}

	std::vector<ModelAndLights*> modelsToDrawAgain;
	modelsToDrawAgain.reserve(100);

	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE bufferData;

	WIPE(bufferData);

	FrameBufferData fData;

	WIPE(fData);

	fData.myWorldToCamera = CommonUtilities::Matrix4x4<float>::Transpose(CommonUtilities::Matrix4x4<float>::GetFastInverse(aCamera->GetTransform()));
	fData.myCameraPosition = aCamera->GetPosition();
	fData.myCameraDirection = aCamera->GetForward();
	fData.myTotalTime = RenderManager::GetTotalTime();
	EnvironmentLight* envoLight = Scene::GetInstance().GetEnvironmentLight();

	if (envoLight)
	{
		const Camera* cam = myDepthRender->GetEnvirontmentCamera();
		fData.myShadowIntensity = myCloudIntensity;
		fData.myWorldToShadowCamera = M44f::GetFastInverse(cam->GetTransform());
		fData.myCameraToShadowProjection = cam->GetProjection();
	}
	myDepthRender->BindshadowsToSlots(9);

	if (envoLight)
	{
		if (envoLight->myTexture.IsValid())
		{
			ID3D11ShaderResourceView* texture[1] =
			{
				envoLight->myTexture.GetAsTexture()
			};

			myContext->PSSetShaderResources(4, 1, texture);
		}
		fData.myEnvironmentLightDirection = V4F(envoLight->myDirection, 0);
		fData.myEnvironmentLightColor = envoLight->myColor;
		fData.myEnviromentLightIntensity = envoLight->myIntensity;
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

	if (mySkyboxTexture.IsValid())
	{
		ID3D11ShaderResourceView* skyboxTexture[1] =
		{
			mySkyboxTexture.GetAsTexture()
		};

		myContext->PSSetShaderResources(3, 1, skyboxTexture);
	}
	else
	{
		ONETIMEWARNING("Rendering without a skybox set","");
	}

	Model* model = nullptr;
	for (size_t i = 0; i < modelsAndLightsList.size(); i++)
	{
		model = modelsAndLightsList[i].first->GetModelAsset().GetAsModel();
		if (!model->ShouldRender())
		{
			continue;
		}

		if (modelsAndLightsList[i].first->ShouldBeDrawnThroughWalls())
		{
			modelsToDrawAgain.push_back(&modelsAndLightsList[i]);
			continue;
		}


		result = myContext->Map(myFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
		if (FAILED(result))
		{
			SYSERROR("Could not map frame buffer");
			return;
		}
		fData.myCameraToProjection = CommonUtilities::Matrix4x4<float>::Transpose(aCamera->GetProjection());
		memcpy(bufferData.pData, &fData, sizeof(fData));
		myContext->Unmap(myFrameBuffer, 0);

		myContext->PSSetShader(model->GetModelData()->myPixelShader.GetAsPixelShader(), nullptr, 0);
		RenderModel(modelsAndLightsList[i].first, modelsAndLightsList[i].second, aBoneMapping, bufferData, aCamera, aBoneBuffer);
	}

	aStateManager.SetDepthStencilState(RenderStateManager::DepthStencilState::OnlyCovered);
	myContext->PSSetShader(myEnemyThroughWallShader.GetAsPixelShader(), nullptr, 0);
	for (size_t i = 0; i < modelsToDrawAgain.size(); i++)
	{
		if (modelsToDrawAgain[i]->first->IsUsingPlayerThroughWallShader())
		{
			myContext->PSSetShader(myThroughWallShader.GetAsPixelShader(), nullptr, 0);
			RenderModel(modelsToDrawAgain[i]->first, modelsToDrawAgain[i]->second, aBoneMapping, bufferData, aCamera, aBoneBuffer);
			myContext->PSSetShader(myEnemyThroughWallShader.GetAsPixelShader(), nullptr, 0);
			continue;
		}


		result = myContext->Map(myFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
		if (FAILED(result))
		{
			SYSERROR("Could not map frame buffer");
			return;
		}
		fData.myCameraToProjection = CommonUtilities::Matrix4x4<float>::Transpose(aCamera->GetProjection());
		memcpy(bufferData.pData, &fData, sizeof(fData));
		myContext->Unmap(myFrameBuffer, 0);
		RenderModel(modelsToDrawAgain[i]->first, modelsToDrawAgain[i]->second, aBoneMapping, bufferData, aCamera, aBoneBuffer);
	}
	aStateManager.SetDepthStencilState(RenderStateManager::DepthStencilState::Default);

	for (size_t i = 0; i < modelsToDrawAgain.size(); i++)
	{
		model = modelsToDrawAgain[i]->first->GetModelAsset().GetAsModel();
		myContext->PSSetShader(model->GetModelData()->myPixelShader.GetAsPixelShader(), nullptr, 0);


		result = myContext->Map(myFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
		if (FAILED(result))
		{
			SYSERROR("Could not map frame buffer");
			return;
		}
		fData.myCameraToProjection = CommonUtilities::Matrix4x4<float>::Transpose(aCamera->GetProjection());
		memcpy(bufferData.pData, &fData, sizeof(fData));
		myContext->Unmap(myFrameBuffer, 0);
		RenderModel(modelsToDrawAgain[i]->first, modelsToDrawAgain[i]->second, aBoneMapping, bufferData, aCamera, aBoneBuffer);
	}
}

void ForwardRenderer::SetSkyboxTexture(AssetHandle aTexture)
{
	mySkyboxTexture = aTexture;
}

void ForwardRenderer::SetSkybox(ModelInstance* aSkyBox)
{
	SetSkyboxTexture(aSkyBox->GetModelAsset().GetAsSkybox()->GetModelData()->myTextures[0]);
	myskybox = aSkyBox;
}

inline void ForwardRenderer::RenderModel(ModelInstance* aModelInstance, std::array<PointLight*, NUMBEROFPOINTLIGHTS>* aLightList, std::unordered_map<ModelInstance*, short>& aBoneMapping, D3D11_MAPPED_SUBRESOURCE& aBuffer, const Camera* aCamera, BoneTextureCPUBuffer& aBoneBuffer)
{
	static Model::ModelData* modelData = nullptr;
	static Model* model = nullptr;
	static ObjectBufferData oData;
	static HRESULT result;
	float now = Tools::GetTotalTime();

	model = aModelInstance->GetModelAsset().GetAsModel();

	modelData = model->GetModelData();

	oData.myModelToWorldSpace = CommonUtilities::Matrix4x4<float>::Transpose(aModelInstance->GetModelToWorldTransformWithPotentialBoneAttachement(aBoneBuffer,aBoneMapping));
	oData.myTint = aModelInstance->GetTint();
	oData.myIsEventActive = ModelInstance::GetEventStatus();
	float lastinteract = aModelInstance->GetLastInteraction();
	if (CLOSEENUF(lastinteract,-1.f))
	{
		oData.myTimeSinceLastInteraction = -1.f;
	}
	else
	{
		oData.myTimeSinceLastInteraction = now - lastinteract;
	}

	if (modelData->myshaderTypeFlags & ShaderFlags::HasBones)
	{
		oData.myBoneOffsetIndex = aBoneMapping[aModelInstance];
	}

	if (aLightList)
	{
		for (size_t i = 0; i < NUMBEROFPOINTLIGHTS; i++)
		{
			if ((*aLightList)[i])
			{
				oData.myPointLights[i].position = (*aLightList)[i]->position;
				oData.myPointLights[i].intensity = (*aLightList)[i]->intensity;
				oData.myPointLights[i].color = (*aLightList)[i]->color;
				oData.myPointLights[i].range = (*aLightList)[i]->range;
			}
			else
			{
				oData.myNumOfUsedPointLights = static_cast<unsigned int>(i);
				break;
			}
		}
	}

	oData.myObjectId = Math::FoldPointer(aModelInstance);
	oData.myObjectLifeTime = Tools::GetTotalTime() - aModelInstance->GetSpawnTime();
	oData.myObjectExpectedLifeTime = aModelInstance->GetExpectedLifeTime();

	WIPE(aBuffer);
	result = myContext->Map(myObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &aBuffer);

	if (FAILED(result))
	{
		SYSERROR("Could not map object buffer");
		return;
	}

	memcpy(aBuffer.pData, &oData, sizeof(oData));
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
	if(modelData->myTextures[0].IsValid()) { resources[0] = modelData->myTextures[0].GetAsTexture(); }
	if(modelData->myTextures[1].IsValid()) { resources[1] = modelData->myTextures[1].GetAsTexture(); }
	if(modelData->myTextures[2].IsValid()) { resources[2] = modelData->myTextures[2].GetAsTexture(); }

	myContext->PSSetShaderResources(0, 3, resources);
	myContext->VSSetShaderResources(0, 3, resources);

	Model::LodLevel* lodlevel = model->GetOptimalLodLevel(aModelInstance->GetPosition().DistanceSqr(aCamera->GetPosition()));
	if (lodlevel)
	{
		myContext->IASetVertexBuffers(0, lodlevel->myVertexBufferCount, lodlevel->myVertexBuffer, &modelData->myStride, &modelData->myOffset);
		myContext->IASetIndexBuffer(lodlevel->myIndexBuffer, modelData->myIndexBufferFormat, 0);
		myContext->DrawIndexed(lodlevel->myNumberOfIndexes, 0, 0);

	}
}