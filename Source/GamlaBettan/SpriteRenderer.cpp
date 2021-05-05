#include "pch.h"
#include "SpriteRenderer.h"
#include "DirectX11Framework.h"
#include "SpriteInstance.h"
#include "Sprite.h"
#include <d3d11.h>
#include "Shaders.h"
#include "ShaderCompiler.h"
#include "StateStack.h"

SpriteRenderer::SpriteRenderer()
{
	myContext = nullptr;
	myObjectBuffer = nullptr;
}

SpriteRenderer::~SpriteRenderer()
{
	myContext = nullptr;


	SAFE_RELEASE(myObjectBuffer);
}

bool SpriteRenderer::Init(DirectX11Framework* aFramework)
{
	if (!aFramework)
	{
		SYSERROR("Framework was nullptr in Init SpriteRenderer","");
		return false;
	}

	myContext = aFramework->GetContext();
	if (!myContext)
	{
		SYSERROR("Framework's context was nullptr in Init SpriteRenderer","");
		return false;
	}

	ID3D11Device* device = aFramework->GetDevice();
	if (!device)
	{
		SYSERROR("Framework's device was nullptr in Init SpriteRenderer","");
		return false;
	}
	HRESULT result;
	D3D11_BUFFER_DESC bufferDescription = { 0 };
	bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	bufferDescription.ByteWidth = sizeof(SObjectBufferData);
	result = device->CreateBuffer(&bufferDescription, nullptr, &myObjectBuffer);

	if (FAILED(result))
	{
		SYSERROR("Failed to create buffer in Init SpriteRenderer","");
		return false;
	}

	std::vector<char> vsBlob;
	myVertexShader = GetVertexShader(device, "Data/Shaders/Sprites/SpriteVertexShader.hlsl", vsBlob);
	if (!myVertexShader)
	{
		SYSERROR("Failed to create vertex shader in SpriteRenderer::Init", "Data/Shaders/Sprites/SpriteVertexShader.hlsl");
		return false;
	}

	myPixelShader = GetPixelShader(device, "Data/Shaders/Sprites/SpritePixelShader.hlsl");
	if (!myPixelShader)
	{
		SYSERROR("Failed to create pixel shader in SpriteRenderer::Init", "Data/Shaders/Sprites/SpritePixelShader.hlsl");
		return false;
	}

	return true;
}

void SpriteRenderer::Render(/*const Camera* aCamera,*/ const std::vector<SpriteInstance*>& aSpriteList)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE bufferData;

	ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
	std::vector<SpriteInstance*> listCopy = aSpriteList;
	std::sort(listCopy.begin(), listCopy.end(), [](SpriteInstance* aLeft, SpriteInstance* aRight) -> bool { return aLeft->GetDepth() > aRight->GetDepth(); });
	

	myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	myContext->IASetInputLayout(nullptr);
	myContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	myContext->VSSetShader(*myVertexShader, nullptr, 0);
	myContext->PSSetShader(*myPixelShader, nullptr, 0);
	
#ifdef _DEBUG
#if USEIMGUI
	static bool debug = false;
	WindowControl::Window("Sprites", [&aSpriteList]()
		{
			ImGui::Text("State: " PFSIZET, StateStack::ourCurrentStateIdentifier);
			ImGui::PushID(StateStack::ourCurrentStateIdentifier);
			ImGui::Checkbox("Debug layout", &debug);
			ImGui::Separator();
			size_t counter = 0;
			for (auto& SpriteInstance : aSpriteList)
			{
				ImGui::PushID(counter);
				if (ImGui::TreeNode("node", "Sprite: " PFSIZET, counter))
				{
					SpriteInstance->ImGui();
					ImGui::Separator();
					ImGui::TreePop();
				}
				ImGui::PopID();
				counter++;
			}
			ImGui::PopID();
		});
	static PixelShader* white = nullptr;
	if (debug)
	{
		if (!white)
		{
			ID3D11Device* device;
			myPixelShader->operator ID3D11PixelShader* ()->GetDevice(&device);
			white = GetPixelShader(device,"Data/Shaders/Sprites/SpritePixelShader_Debug.hlsl");
		}

		myContext->PSSetShader(*white, nullptr, 0);
	}
#endif // 
#endif // _DEBUG

	
	for (SpriteInstance* spriteInstance : listCopy)
	{
		const Sprite::SpriteData* spriteData = &spriteInstance->GetSprite()->GetSpriteData();

		myObjectBufferData.myPivotToModel = spriteInstance->GetPivotTransform();
		myObjectBufferData.myModelToWorld = spriteInstance->GetTransform();
		
		myObjectBufferData.myColor = spriteInstance->GetColor();

#ifdef _DEBUG
#if USEIMGUI
		if (debug)
		{
			size_t seed = std::hash<size_t>()(reinterpret_cast<size_t>(spriteData));
			srand(seed);
			myObjectBufferData.myColor = V4F(rand() / double(RAND_MAX), rand() / double(RAND_MAX), rand() / double(RAND_MAX), 0.5f);
		}
#endif // 
#endif // _DEBUG

		myObjectBufferData.myUVMinMax = spriteInstance->GetUVMinMax();
		myObjectBufferData.myDepth = spriteInstance->GetDepth();

		ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
		result = myContext->Map(myObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
		if (FAILED(result))
		{
			SYSERROR("myContext->Map failed in Render SpriteRenderer","");
			return;
		}

		memcpy(bufferData.pData, &myObjectBufferData, sizeof(SObjectBufferData));
		myContext->Unmap(myObjectBuffer, 0);

		myContext->VSSetConstantBuffers(0, 1, &myObjectBuffer);
		myContext->PSSetConstantBuffers(0, 1, &myObjectBuffer);
		myContext->PSSetShaderResources(0, 1, *spriteData->myTexture);

		myContext->Draw(6, 0);
	}
}
