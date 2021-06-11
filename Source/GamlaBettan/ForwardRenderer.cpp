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

bool ForwardRenderer::Init(DirectX11Framework* aFramework, AssetHandle aPerlinHandle, DepthRenderer* aDepthRenderer)
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

	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE bufferData;

	WIPE(bufferData);

	FrameBufferData fData;

	WIPE(fData);

	fData.myWorldToCamera = M44f::Transpose(M44f::GetFastInverse(aCamera->GetTransform()));
	fData.myCameraPosition = aCamera->GetPosition();
	fData.myCameraDirection = aCamera->GetForward();
	fData.myTotalTime = RenderManager::GetTotalTime();
	fData.myCameraToProjection = M44f::Transpose(aCamera->GetProjection());

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

	for (ModelAndLights& modelAndLight : modelsAndLightsList)
	{
		Model* model = modelAndLight.first->GetModelAsset().GetAsModel();
		if (!model->ShouldRender())
		{
			continue;
		}

		result = myContext->Map(myFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
		if (FAILED(result))
		{
			SYSERROR("Could not map frame buffer");
			return;
		}
		memcpy(bufferData.pData, &fData, sizeof(fData));
		myContext->Unmap(myFrameBuffer, 0);

		RenderModel(modelAndLight.first, modelAndLight.second, aBoneMapping, bufferData, aCamera, aBoneBuffer);
	}

	aStateManager.SetDepthStencilState(RenderStateManager::DepthStencilState::OnlyCovered);
}

void ForwardRenderer::SetSkyboxTexture(AssetHandle aTexture)
{
	mySkyboxTexture = aTexture;
}

void ForwardRenderer::SetSkybox(ModelInstance* aSkyBox)
{
	SetSkyboxTexture(aSkyBox->GetModelAsset().GetAsSkybox()->GetModelData()[0]->myTextures[0]);
	myskybox = aSkyBox;
}

inline void ForwardRenderer::RenderModel(
	ModelInstance* aModelInstance, std::array<PointLight*, NUMBEROFPOINTLIGHTS>* aLightList, std::unordered_map<ModelInstance*, short>& aBoneMapping, 
	D3D11_MAPPED_SUBRESOURCE& aBuffer, const Camera* aCamera, BoneTextureCPUBuffer& aBoneBuffer)
{
	static HRESULT result;
	float now = Tools::GetTotalTime();

	Model* model = aModelInstance->GetModelAsset().GetAsModel();

	for (Model::ModelData* modelData : model->GetModelData())
	{

		ObjectBufferData oData;
		WIPE(oData);
		oData.myModelToWorldSpace =
			M44f::Transpose(aModelInstance->GetModelToWorldTransformWithPotentialBoneAttachement(aBoneBuffer, aBoneMapping))
			* modelData->myOffset;
		oData.myTint = aModelInstance->GetTint();
		oData.myDiffuseColor = modelData->myDiffuseColor;

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

		oData.myObjectId = Math::BinaryFold(aModelInstance);
		oData.myObjectLifeTime = Tools::GetTotalTime() - aModelInstance->GetSpawnTime();

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
		myContext->PSSetConstantBuffers(1, 1, &myObjectBuffer);
		myContext->VSSetShader(modelData->myVertexShader.GetAsVertexShader(), nullptr, 0);
		myContext->PSSetShader(modelData->myPixelShader.GetAsPixelShader(), nullptr, 0);


		ID3D11ShaderResourceView* resources[3] = { nullptr };

		if (modelData->myTextures[0].IsValid()) { resources[0] = modelData->myTextures[0].GetAsTexture(); }
		if (modelData->myTextures[1].IsValid()) { resources[1] = modelData->myTextures[1].GetAsTexture(); }
		if (modelData->myTextures[2].IsValid()) { resources[2] = modelData->myTextures[2].GetAsTexture(); }

		myContext->PSSetShaderResources(0, 3, resources);
		myContext->VSSetShaderResources(0, 3, resources);

		UINT bufferOffset = 0;

		myContext->IASetVertexBuffers(0, 1, &modelData->myVertexBuffer, &modelData->myStride, &bufferOffset);
		myContext->IASetIndexBuffer(modelData->myIndexBuffer, modelData->myIndexBufferFormat, 0);
		myContext->DrawIndexed(modelData->myNumberOfIndexes, 0, 0);
	}
}