#include "pch.h"
#include "ParticleRenderer.h"
#include <d3d11.h>
#include "Camera.h"
#include "DirectX11Framework.h"
#include "PostMaster.hpp"
#include "TextureLoader.h"

struct ParticleFrameBufferData
{
	CommonUtilities::Matrix4x4<float> myWorldToCamera;
	CommonUtilities::Matrix4x4<float> myCameraToProjection;
	CommonUtilities::Vector3<float> myCameraForward;
	float myTotalTime = 0.f;
};

struct ParticleObjectBufferData
{
	CommonUtilities::Matrix4x4<float> myPointToWorldSpace;
};

bool ParticleRenderer::Init(DirectX11Framework* aFramework)
{
	myFrameWork = aFramework;
	ID3D11Device* device = aFramework->GetDevice();
	D3D11_BUFFER_DESC desc;
	WIPE(desc);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(ParticleFrameBufferData);
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;

	HRESULT result = device->CreateBuffer(&desc, nullptr, &myFrameBuffer);
	if (FAILED(result))
	{
		SYSERROR("Could no oh no.");
		return false;
	}
	desc.ByteWidth = sizeof(ParticleObjectBufferData);
	result = device->CreateBuffer(&desc, nullptr, &myObjectBuffer);
	if (FAILED(result))
	{
		SYSERROR("Shit's fucked.");
		return false;
	}

	return true;
}

void ParticleRenderer::Render(Camera* aCamera,const std::vector<ParticleInstance*>& aParticleList)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE bufferData;
	ParticleFrameBufferData frameBufferData;

	ID3D11DeviceContext* context = myFrameWork->GetContext();

	frameBufferData.myWorldToCamera = CommonUtilities::Matrix4x4<float>::GetFastInverse(aCamera->GetTransform());
	frameBufferData.myCameraToProjection = aCamera->GetProjection();


	WIPE(bufferData);
	result = context->Map(myFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
	if (FAILED(result))
	{
		SYSERROR("Oh no, partcle ded");
		return;
	}

	memcpy(bufferData.pData, &frameBufferData, sizeof(ParticleFrameBufferData));
	context->Unmap(myFrameBuffer, 0);
	context->VSSetConstantBuffers(0, 1, &myFrameBuffer);
	context->GSSetConstantBuffers(0, 1, &myFrameBuffer);

	ParticleInstance* instance;
	Particle* particle;
	ParticleObjectBufferData objectBufferData;
	Particle::Data particleData;

	for (size_t index = 0; index < aParticleList.size(); index++)
	{
		instance = aParticleList[index];
		particle = instance->GetParticle();

		particleData = particle->GetData();

		objectBufferData.myPointToWorldSpace = instance->GetTransform();

		//if (particleData.myCustomizable.myFlipBook.myPages > 0)
		//{
		//	aParticleList[index]->GetParticleVertices()[0].
		//	objectBufferData.myUVMin = particleData.myCustomizable.myFlipBook.myUVMin;
		//	objectBufferData.myUVMax = particleData.myCustomizable.myFlipBook.myUVMax;
		//}

		WIPE(bufferData);

		result = context->Map(myObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
		if (FAILED(result))
		{
			SYSERROR("Oh no, partcle ded");
			return;
		}
		memcpy(bufferData.pData, &objectBufferData, sizeof(objectBufferData));
		context->Unmap(myObjectBuffer, 0);


		WIPE(bufferData);
		result = context->Map(particleData.myParticleVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
		if (FAILED(result))
		{
			SYSERROR("Oh no, partcle ded");
			return;
		}
		const std::vector<Particle::Vertex>& vertexList = instance->GetParticleVertices();

		unsigned int count = CAST(unsigned int, MIN(instance->GetParticle()->GetData().myCustomizable.mySpawnRate * instance->GetParticle()->GetData().myCustomizable.myParticleLifetime, vertexList.size()));
		if (!vertexList.empty())
		{
			memcpy(bufferData.pData, vertexList.data(), sizeof(Particle::Vertex) * count);
		}
		context->Unmap(particleData.myParticleVertexBuffer, 0);

		context->IASetPrimitiveTopology(particleData.myPrimitiveTopology);
		context->IASetInputLayout(particleData.myInputLayout);
		context->IASetVertexBuffers(0, 1, &particleData.myParticleVertexBuffer, &particleData.myStride, &particleData.myOffset);

		context->VSSetConstantBuffers(1, 1, &myObjectBuffer);
		context->VSSetShader(particleData.myVertexShader.GetAsVertexShader(), nullptr, 0);

		context->GSSetConstantBuffers(1, 1, &myObjectBuffer);
		context->GSSetShader(particleData.myGeometryShader.GetAsGeometryShader(), nullptr, 0);

		context->PSSetConstantBuffers(1, 1, &myObjectBuffer);
		ID3D11ShaderResourceView* texture[1] =
		{
			particleData.myTexture.GetAsTexture()
		};
		context->PSSetShaderResources(0, 1, texture);

		context->PSSetShader(particleData.myPixelShader.GetAsPixelShader(), nullptr, 0);

		context->Draw(count, 0);
	}
}