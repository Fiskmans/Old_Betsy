#include "pch.h"
#include "HighlightRenderer.h"
#include "ShaderBuffers.h"
#include "ShaderCompiler.h"
#include "Model.h"
#include "ModelInstance.h"
#include "TimeHelper.h"
#include "Camera.h"
#include "DirectX11Framework.h"

bool HighlightRenderer::Init(DirectX11Framework* aFramework)
{
	if (!aFramework)
	{
		SYSCRASH("HighlightRenderer was not given a framework to work on");
		return false;
	}

	myContext = aFramework->GetContext();
	if (!myContext)
	{
		SYSCRASH("HighlightRenderer could not get context from framework");
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

	const size_t buffercount = 2;

	ID3D11Buffer** buffers[buffercount] = {
		&myFrameBuffer,
		&myObjectBuffer,};
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

	myPixelShader = GetPixelShader(device, "Data/Shaders/FlatColor.hlsl");
	if (!myPixelShader)
	{
		SYSCRASH("Could not compile pixel shader for deferred");
		return false;
	}

	myCreateTime = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch().count();
	return true;
}

void HighlightRenderer::Render(const std::vector<class ModelInstance*>& aModels,class Camera* aCamera, std::unordered_map<ModelInstance*, short>& aBoneMapping)
{
	long long now = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch().count();
	static float lastTime = 0;
	float totalTime = static_cast<float>(now - myCreateTime) * 0.001f;
	float deltatime = totalTime - lastTime;
	lastTime = deltatime;

	std::vector<class ModelInstance*> filtered;
	Model* model = nullptr;
	Model::CModelData* modelData = nullptr;


	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE bufferData;

	WIPE(bufferData);

	FrameBufferData fData;

	WIPE(fData);

	fData.myWorldToCamera = CommonUtilities::Matrix4x4<float>::Transpose(CommonUtilities::Matrix4x4<float>::GetFastInverse(aCamera->GetTransform()));
	fData.myCameraPosition = aCamera->GetPosition();
	fData.myTotalTime = totalTime;



	myContext->VSSetConstantBuffers(0, 1, &myFrameBuffer);
	myContext->PSSetConstantBuffers(0, 1, &myFrameBuffer);
	myContext->PSSetShader(*myPixelShader, nullptr, 0);
	myContext->GSSetShader(nullptr, nullptr, 0);

	ObjectBufferData oData;
	for (size_t i = 0; i < aModels.size(); i++)
	{

		model = aModels[i]->GetModel();
		if (!model->ShouldRender())
		{
			continue;
		}
		modelData = model->GetModelData();

		fData.myCameraToProjection = CommonUtilities::Matrix4x4<float>::Transpose(aCamera->GetProjection(aModels[i]->GetIsUsingSecondaryFov()));
		result = myContext->Map(myFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
		if (FAILED(result))
		{
			SYSERROR("Could not map frame buffer", "");
			return;
		}
		memcpy(bufferData.pData, &fData, sizeof(fData));
		myContext->Unmap(myFrameBuffer, 0);


		oData.myModelToWorldSpace = CommonUtilities::Matrix4x4<float>::Transpose(aModels[i]->GetModelToWorldTransform());
		oData.myTint = aModels[i]->GetTint();
		if (modelData->myshaderTypeFlags & ShaderFlags::HasBones)
		{
			oData.myBoneOffsetIndex = aBoneMapping[aModels[i]];
		}

		oData.myObjectLifeTime = Tools::GetTotalTime() - aModels[i]->GetSpawnTime();
		oData.myObjectExpectedLifeTime = aModels[i]->GetExpectedLifeTime();

		WIPE(bufferData);
		result = myContext->Map(myObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);

		if (FAILED(result))
		{
			SYSERROR("Could not map object buffer","");
			return;
		}

		memcpy(bufferData.pData, &oData, sizeof(oData));
		myContext->Unmap(myObjectBuffer, 0);



		myContext->IASetPrimitiveTopology(modelData->myPrimitiveTopology);
		myContext->IASetInputLayout(modelData->myInputLayout);


		myContext->VSSetConstantBuffers(1, 1, &myObjectBuffer);
		myContext->VSSetShader(*modelData->myVertexShader, nullptr, 0);

		myContext->PSSetConstantBuffers(1, 1, &myObjectBuffer);
		myContext->PSSetShaderResources(0, 1, *modelData->myTextures[0]);
		myContext->PSSetShaderResources(1, 1, *modelData->myTextures[1]);
		myContext->PSSetShaderResources(2, 1, *modelData->myTextures[2]);
		myContext->VSSetShaderResources(0, 1, *modelData->myTextures[0]);
		myContext->VSSetShaderResources(1, 1, *modelData->myTextures[1]);
		myContext->VSSetShaderResources(2, 1, *modelData->myTextures[2]);

		Model::LodLevel* lodlevel = model->GetOptimalLodLevel(aModels[i]->GetPosition().Distance(aCamera->GetPosition()));
		if (lodlevel)
		{
			myContext->IASetVertexBuffers(0, lodlevel->myVertexBufferCount, lodlevel->myVertexBuffer, &modelData->myStride, &modelData->myOffset);
			myContext->IASetIndexBuffer(lodlevel->myIndexBuffer, modelData->myIndexBufferFormat, 0);
			myContext->DrawIndexed(lodlevel->myNumberOfIndexes, 0, 0);
		}
		else
		{
			//SYSWARNING("Rendered without any loaded lod levels");
		}
	}
}
