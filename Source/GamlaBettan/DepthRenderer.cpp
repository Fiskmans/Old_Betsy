#include "pch.h"
#include "DepthRenderer.h"
#include "CameraFactory.h"
#include "DirectX11Framework.h"
#include "PointLight.h"
#include "Model.h"
#include "ShaderTypes.h"
#include "ShaderFlags.h"
#include "ShaderBuffers.h"
#include "ShaderCompiler.h"
#include "SpotLight.h"
#include "Decal.h"
#include "Environmentlight.h"
#include "RenderManager.h"
#include "AssetManager.h"
#include "GamlaBettan\Scene.h"
#include "GamlaBettan\ModelInstance.h"

DepthRenderer::~DepthRenderer()
{
}

bool DepthRenderer::Init(DirectX11Framework* aFramework)
{
	ID3D11Device* device = aFramework->GetDevice();
	myContext = aFramework->GetContext();
	for (size_t i = 0; i < 6; i++)
	{
		myCameras[i] = CCameraFactory::CreateCamera(90, false, SHADOWNEARPLANE, SHADOWFARPLANE);
		myCameras[i]->SetResolution(V2ui(SHADOWRESOLUTION, SHADOWRESOLUTION));
	}
	myCameras[0]->SetRotation(V3F(0, 0, 0));
	myCameras[1]->SetRotation(V3F(PI, 0, 0));
	myCameras[2]->SetRotation(V3F(-PI / 2, 0, 0));
	myCameras[3]->SetRotation(V3F(PI / 2, 0, 0));
	myCameras[4]->SetRotation(V3F(0, -PI / 2, 0));
	myCameras[5]->SetRotation(V3F(0, PI / 2, 0));

	myEnvironmentCamera = CCameraFactory::CreateCamera(Camera::CameraType::Orthographic);

	//myEnvironmentCamera = CCameraFactory::CreateCamera(90, false, SHADOWNEARPLANE, SHADOWFARPLANE);
	//myEnvironmentCamera->SetResolution(V2F(SHADOWRESOLUTION, SHADOWRESOLUTION));
	
	
	HRESULT result;

	myShader = AssetManager::GetInstance().GetPixelShader("White.hlsl", ShaderFlags::None);

	ID3D11Texture2D* tex;
	ID3D11Texture2D* texEnvironment;
	ID3D11Texture2D* texDecal;
	ID3D11ShaderResourceView* view;
	ID3D11ShaderResourceView* viewEnvironment;
	ID3D11DepthStencilView* depth;
	ID3D11DepthStencilView* depthEnvironment;
	ID3D11DepthStencilView* depthDecals;

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

	myDevice = device;

	DirectX11Framework::AddGraphicsMemoryUsage(static_cast<size_t>(tdesc.Height * tdesc.Width * DirectX11Framework::FormatToSizeLookup[DXGI_FORMAT_R32_TYPELESS]), "Poitlight Depth", "Engine Depth");
	result = device->CreateTexture2D(&tdesc, nullptr, &tex);
	if (FAILED(result))
	{
		SYSERROR("Could not shadow depth texture", "pointlights");
		return false;
	}

	tdesc.Width = ENVIRONMENTSHADOWRESOLUTION;
	tdesc.Height = ENVIRONMENTSHADOWRESOLUTION;
	DirectX11Framework::AddGraphicsMemoryUsage(static_cast<size_t>(tdesc.Height * tdesc.Width * DirectX11Framework::FormatToSizeLookup[DXGI_FORMAT_R32_TYPELESS]), "Environment Depth", "Engine Depth");
	result = device->CreateTexture2D(&tdesc, nullptr, &texEnvironment);
	if (FAILED(result))
	{
		SYSERROR("Could not shadow depth texture", "Environment");
		return false;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	WIPE(dsvDesc);
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;

	result = device->CreateDepthStencilView(tex, &dsvDesc, &depth);
	if (FAILED(result))
	{
		SYSERROR("Could not shadow depth view", "pointlights");
		return false;
	}

	result = device->CreateDepthStencilView(texEnvironment, &dsvDesc, &depthEnvironment);
	if (FAILED(result))
	{
		SYSERROR("Could not shadow depth view", "Environment");
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

	result = device->CreateShaderResourceView(texEnvironment, &srvDesc, &viewEnvironment);
	texEnvironment->Release();
	if (FAILED(result))
	{
		SYSERROR("Could not shadow depth resource view");
		return false;
	}

	myDepth1x6 = depth;
	myDepthsResource1x6 = view;

	myDepth1x1 = depthEnvironment;
	myDepthsResource1x1 = viewEnvironment;

	ID3D11Texture2D* tex2_1x6;
	ID3D11Texture2D* tex2_1x1;
	CD3D11_TEXTURE2D_DESC desc2;
	WIPE(desc2);
	desc2.BindFlags = D3D11_BIND_RENDER_TARGET;
	desc2.Usage = D3D11_USAGE_DEFAULT;

	desc2.Format = DXGI_FORMAT_R8_UNORM;
	desc2.Height = SHADOWRESOLUTION;
	desc2.Width = SHADOWRESOLUTION * 6;
	desc2.ArraySize = 1;
	desc2.MipLevels = 1;
	desc2.SampleDesc.Count = 1;
	desc2.SampleDesc.Quality = 0;

	DirectX11Framework::AddGraphicsMemoryUsage(desc2.Height * desc2.Width * DirectX11Framework::FormatToSizeLookup[desc2.Format], "pointlight rt", "Engine Texture");
	result = device->CreateTexture2D(&desc2, nullptr, &tex2_1x6);
	if (FAILED(result))
	{
		SYSERROR("Could not shadow depth target texture");
		return false;
	}

	desc2.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc2.Width = ENVIRONMENTSHADOWRESOLUTION;
	desc2.Height = ENVIRONMENTSHADOWRESOLUTION;

	DirectX11Framework::AddGraphicsMemoryUsage(desc2.Height * desc2.Width * DirectX11Framework::FormatToSizeLookup[desc2.Format], "environment rt", "Engine Texture");
	result = device->CreateTexture2D(&desc2, nullptr, &tex2_1x1);
	if (FAILED(result))
	{
		SYSERROR("Could not shadow depth target texture");
		return false;
	}

	desc2.Height = DECALRESOLUTION;
	desc2.Width = DECALRESOLUTION;

	DirectX11Framework::AddGraphicsMemoryUsage(desc2.Height * desc2.Width * DirectX11Framework::FormatToSizeLookup[desc2.Format], "decal rt", "Engine Texture");
	result = device->CreateTexture2D(&desc2, nullptr, &texDecal);
	if (FAILED(result))
	{
		SYSERROR("Could not shadow depth texture", "decals");
		return false;
	}

	result = device->CreateRenderTargetView(tex2_1x6, nullptr, &myRenderTarget1x6);
	if (FAILED(result))
	{
		SYSERROR("Could not shadow depth target view", "pointlights");
		return false;
	}
	result = device->CreateRenderTargetView(texDecal, nullptr, &myDecalRenderTarget);
	if (FAILED(result))
	{
		SYSERROR("Could not shadow depth target view", "decal");
		return false;
	}

	result = device->CreateRenderTargetView(tex2_1x1, nullptr, &myRenderTarget1x1);
	if (FAILED(result))
	{
		SYSERROR("Could not shadow depth target view", "Environment");
		return false;
	}
	tex2_1x6->Release();
	texDecal->Release();

	result = device->CreateShaderResourceView(tex2_1x1, nullptr, &myRenderTargetResourceView1x1);
	if (FAILED(result))
	{
		SYSERROR("Could not shadow render target resource view", "Environment");
		return false;
	}
	tex2_1x1->Release();

	D3D11_BUFFER_DESC bufferDescription;
	ZeroMemory(&bufferDescription, sizeof(bufferDescription));
	bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	const size_t buffercount = 2;

	ID3D11Buffer** buffers[buffercount] = {
		&myFrameBuffer,
		&myObjectBuffer };
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

const std::array<Camera*, 6>& DepthRenderer::GetCameras()
{
	return myCameras;
}

const Camera* DepthRenderer::GetEnvirontmentCamera()
{
	return myEnvironmentCamera;
}

void DepthRenderer::BindshadowsToSlots(int aSlot)
{
	myContext->PSSetShaderResources(aSlot, 1, &myLastDepthView);
}

void DepthRenderer::BindShadowDensityToSlot(int aSlot)
{
	myContext->PSSetShaderResources(aSlot, 1, &myRenderTargetResourceView1x1);
}

void DepthRenderer::Render(PointLight* aLight, BoneTextureCPUBuffer& aBoneBuffer, std::unordered_map<ModelInstance*, short>& aBoneMapping)
{
	std::vector<ModelInstance*> preCull;
	{
		PERFORMANCETAG("Precull");
		preCull = Scene::GetInstance().Cull(CommonUtilities::Sphere<float>(aLight->position, SHADOWFARPLANE * CUBEHALFSIZETOENCAPSULATINGSPHERERADIUS));
	}


	myContext->PSSetShader(myShader.GetAsPixelShader(), nullptr, 0);
	myContext->OMSetRenderTargets(1, &myRenderTarget1x6, myDepth1x6);

	myContext->ClearDepthStencilView(myDepth1x6, D3D11_CLEAR_DEPTH, 1.f, 0);
	for (size_t i = 0; i < 6; i++)
	{
		D3D11_VIEWPORT port;
		WIPE(port);
		port.Height = SHADOWRESOLUTION;
		port.Width = SHADOWRESOLUTION;
		port.MinDepth = 0.f;
		port.MaxDepth = 1.f;
		port.TopLeftX = SHADOWRESOLUTION * i;
		port.TopLeftY = 0;
		myContext->RSSetViewports(1, &port);
		myCameras[i]->SetPosition(aLight->position);
		std::vector<class ModelInstance*> filtered;
		{
			PERFORMANCETAG("Culling");
			filtered = Scene::GetInstance().Cull(myCameras[i], preCull, false);
		}

		Render(myCameras[i], filtered, aBoneBuffer, aBoneMapping);
	}
}

void DepthRenderer::RenderSpotLightDepth(SpotLight* aSpotlight, BoneTextureCPUBuffer& aBoneBuffer, std::unordered_map<ModelInstance*, short>& aBoneMapping)
{
	myContext->ClearDepthStencilView(myDepth1x6, D3D11_CLEAR_DEPTH, 1.f, 0);

	D3D11_VIEWPORT port;
	WIPE(port);
	port.Height = SHADOWRESOLUTION;
	port.Width = SHADOWRESOLUTION;
	port.MinDepth = 0.f;
	port.MaxDepth = 1.f;
	port.TopLeftX = 0;
	port.TopLeftY = 0;
	myContext->PSSetShader(myShader.GetAsPixelShader(), nullptr, 0);
	myContext->RSSetViewports(1, &port);
	myContext->OMSetRenderTargets(1, &myRenderTarget1x6, myDepth1x6);

	std::vector<class ModelInstance*> filtered;
	{
		PERFORMANCETAG("Culling");
		filtered = Scene::GetInstance().Cull(aSpotlight->myCamera);
	}

	Render(aSpotlight->myCamera, filtered, aBoneBuffer, aBoneMapping);
	myLastDepthView = myDepthsResource1x6;
}

void DepthRenderer::RenderEnvironmentDepth(EnvironmentLight* aLight, BoneTextureCPUBuffer& aBoneBuffer, std::unordered_map<ModelInstance*, short>& aBoneMapping, std::function<void()> aPreRenderFunction)
{
	if (!aLight)
	{
		ONETIMEWARNING("Rendering without an envinronmentlight");
		return;
	}
	myContext->ClearDepthStencilView(myDepth1x1, D3D11_CLEAR_DEPTH, 1.f, 0);
	float clearColor[4] = { 0.f,0.f,0.f,0.f };
	myContext->ClearRenderTargetView(myRenderTarget1x1,clearColor);

	D3D11_VIEWPORT port;
	WIPE(port);
	port.Height = ENVIRONMENTSHADOWRESOLUTION;
	port.Width = ENVIRONMENTSHADOWRESOLUTION;
	port.MinDepth = 0.f;
	port.MaxDepth = 1.f;
	port.TopLeftX = 0;
	port.TopLeftY = 0;
	myContext->RSSetViewports(1, &port);
	myContext->OMSetRenderTargets(1, &myRenderTarget1x1, myDepth1x1);
	
	myEnvironmentCamera->SetPosition(aLight->myShadowCorePosition + aLight->myDirection * ENVIRONMENCAMEREASIZE*0.5f);
	myEnvironmentCamera->LookAt(aLight->myShadowCorePosition);

	std::vector<class ModelInstance*> filtered;
	{
		PERFORMANCETAG("Culling");
		filtered = Scene::GetInstance().Cull(myEnvironmentCamera);
	}


	if (aPreRenderFunction)
	{
		aPreRenderFunction();
	}
	myContext->PSSetShader(myShader.GetAsPixelShader(), nullptr, 0);
	Render(myEnvironmentCamera, filtered, aBoneBuffer, aBoneMapping);
	myLastDepthView = myDepthsResource1x1;
}

ID3D11ShaderResourceView* DepthRenderer::RenderDecalDepth(Decal* aDecal, BoneTextureCPUBuffer& aBoneBuffer, std::unordered_map<ModelInstance*, short>& aBoneMapping)
{
	ID3D11ShaderResourceView* view;
	ID3D11DepthStencilView* depth;
	ID3D11Texture2D* tex;
	HRESULT result;

	CD3D11_TEXTURE2D_DESC tdesc;
	WIPE(tdesc);
	tdesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	tdesc.Usage = D3D11_USAGE_DEFAULT;

	tdesc.Format = DXGI_FORMAT_R32_TYPELESS;
	tdesc.Height = DECALRESOLUTION;
	tdesc.Width = DECALRESOLUTION;
	tdesc.ArraySize = 1;
	tdesc.MipLevels = 1;
	tdesc.SampleDesc.Count = 1;
	tdesc.SampleDesc.Quality = 0;


	result = myDevice->CreateTexture2D(&tdesc, nullptr, &tex);
	if (FAILED(result))
	{
		SYSERROR("Could not shadow depth texture");
		return nullptr;
	}


	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	WIPE(dsvDesc);
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;

	result = myDevice->CreateDepthStencilView(tex, &dsvDesc, &depth);
	if (FAILED(result))
	{
		SYSERROR("Could not shadow depth view");
		return nullptr;
	}


	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	WIPE(srvDesc);
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	result = myDevice->CreateShaderResourceView(tex, &srvDesc, &view);
	if (FAILED(result))
	{
		SYSERROR("Could not shadow depth resource view");
		return nullptr;
	}


	myContext->ClearDepthStencilView(depth, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	D3D11_VIEWPORT port;
	WIPE(port);
	port.Height = DECALRESOLUTION;
	port.Width = DECALRESOLUTION;
	port.MinDepth = 0.f;
	port.MaxDepth = 1.f;
	port.TopLeftX = 0;
	port.TopLeftY = 0;
	myContext->PSSetShader(myShader.GetAsPixelShader(), nullptr, 0);
	myContext->RSSetViewports(1, &port);
	myContext->OMSetRenderTargets(1, &myDecalRenderTarget, depth);

	std::vector<class ModelInstance*> filtered = Scene::GetInstance().Cull(aDecal->myCamera);
	Render(aDecal->myCamera, filtered, aBoneBuffer, aBoneMapping);

	tex->Release();
	depth->Release();

	return view;
}

void DepthRenderer::Render(Camera* aCamera, const std::vector<ModelInstance*>& aModelList, BoneTextureCPUBuffer& aBoneBuffer, std::unordered_map<ModelInstance*, short>& aBoneMapping)
{
	PERFORMANCETAG("DepthRender");
	Model* model = nullptr;
	Model::ModelData* modelData = nullptr;


	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE bufferData;


	FrameBufferData fData;
	WIPE(fData);
	fData.myWorldToCamera = M44f::Transpose(M44f::GetFastInverse(aCamera->GetTransform()));
	fData.myCameraToProjection = M44f::Transpose(aCamera->GetProjection());
	fData.myCameraPosition = aCamera->GetPosition();
	fData.myTotalTime = RenderManager::GetTotalTime();



	WIPE(bufferData);
	result = myContext->Map(myFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
	if (FAILED(result))
	{
		SYSERROR("Could not map frame buffer");
		return;
	}
	memcpy(bufferData.pData, &fData, sizeof(fData));
	myContext->Unmap(myFrameBuffer, 0);

	myContext->VSSetConstantBuffers(0, 1, &myFrameBuffer);
	myContext->PSSetConstantBuffers(0, 1, &myFrameBuffer);
	myContext->GSSetShader(nullptr, nullptr, 0);

	const float now = Tools::GetTotalTime();

	{
		PERFORMANCETAG("Render");
		for (ModelInstance* modelInstance : aModelList)
		{
			model = modelInstance->GetModelAsset().GetAsModel();
			if (!model->ShouldRender() || !modelInstance->GetCastsShadows())
			{
				continue;
			}
			if (model->ShouldRenderWithForwardRenderer())
			{
				continue;
			}

			for (Model::ModelData* modelData : model->GetModelData())
			{

				ObjectBufferData oData;
				WIPE(oData);
				oData.myModelToWorldSpace =
					M44f::Transpose(modelInstance->GetModelToWorldTransformWithPotentialBoneAttachement(aBoneBuffer, aBoneMapping))
					* modelData->myOffset;
				oData.myTint = modelInstance->GetTint();
				if (modelData->myshaderTypeFlags & ShaderFlags::HasBones)
				{
					oData.myBoneOffsetIndex = aBoneMapping[modelInstance];
				}

				oData.myObjectLifeTime = now - modelInstance->GetSpawnTime();
				oData.myDiffuseColor = modelData->myDiffuseColor;

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
				myContext->IASetIndexBuffer(modelData->myIndexBuffer, modelData->myIndexBufferFormat, 0);

				myContext->DrawIndexed(modelData->myNumberOfIndexes, 0, 0);
			}
		}
	}
}

