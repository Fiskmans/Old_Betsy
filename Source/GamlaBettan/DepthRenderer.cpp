#include "pch.h"
#include "DepthRenderer.h"
#include "CameraFactory.h"
#include "DirectX11Framework.h"
#include "PointLight.h"
#include "Model.h"
#include "ShaderTypes.h"
#include "ShaderFlags.h"
#include "ShaderBuffers.h"
#include "Shaders.h"
#include "ShaderCompiler.h"
#include "SpotLight.h"
#include "Decal.h"
#include "Environmentlight.h"
#include "RenderManager.h"

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
		myCameras[i]->SetResolution(V2F(SHADOWRESOLUTION, SHADOWRESOLUTION));
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

	myShader = GetPixelShader(device, "Data/Shaders/White.hlsl", ShaderFlags::None);
	if (!myShader)
	{
		SYSERROR("could not compile White shader for shadowrender.", "");
		return false;
	}

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

	DirectX11Framework::AddMemoryUsage(tdesc.Height * tdesc.Width * DirectX11Framework::FormatToSizeLookup[DXGI_FORMAT_R32_TYPELESS], "Poitlight Depth", "Engine Depth");
	result = device->CreateTexture2D(&tdesc, nullptr, &tex);
	if (FAILED(result))
	{
		SYSERROR("Could not shadow depth texture", "pointlights");
		return false;
	}

	tdesc.Width = ENVIRONMENTSHADOWRESOLUTION;
	tdesc.Height = ENVIRONMENTSHADOWRESOLUTION;
	DirectX11Framework::AddMemoryUsage(tdesc.Height * tdesc.Width * DirectX11Framework::FormatToSizeLookup[DXGI_FORMAT_R32_TYPELESS], "Environment Depth", "Engine Depth");
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
		SYSERROR("Could not shadow depth resource view", "");
		return false;
	}

	result = device->CreateShaderResourceView(texEnvironment, &srvDesc, &viewEnvironment);
	texEnvironment->Release();
	if (FAILED(result))
	{
		SYSERROR("Could not shadow depth resource view", "");
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

	DirectX11Framework::AddMemoryUsage(desc2.Height * desc2.Width * DirectX11Framework::FormatToSizeLookup[desc2.Format], "pointlight rt", "Engine Texture");
	result = device->CreateTexture2D(&desc2, nullptr, &tex2_1x6);
	if (FAILED(result))
	{
		SYSERROR("Could not shadow depth target texture", "");
		return false;
	}

	desc2.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc2.Width = ENVIRONMENTSHADOWRESOLUTION;
	desc2.Height = ENVIRONMENTSHADOWRESOLUTION;

	DirectX11Framework::AddMemoryUsage(desc2.Height * desc2.Width * DirectX11Framework::FormatToSizeLookup[desc2.Format], "environment rt", "Engine Texture");
	result = device->CreateTexture2D(&desc2, nullptr, &tex2_1x1);
	if (FAILED(result))
	{
		SYSERROR("Could not shadow depth target texture", "");
		return false;
	}

	desc2.Height = DECALRESOLUTION;
	desc2.Width = DECALRESOLUTION;

	DirectX11Framework::AddMemoryUsage(desc2.Height * desc2.Width * DirectX11Framework::FormatToSizeLookup[desc2.Format], "decal rt", "Engine Texture");
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

void DepthRenderer::Render(PointLight* aLight, Scene* aScene, std::unordered_map<ModelInstance*, short>& aBoneMapping)
{
	std::vector<ModelInstance*> preCull;
	{
		PERFORMANCETAG("Precull");
		preCull = aScene->Cull(CommonUtilities::Sphere<float>(aLight->position, SHADOWFARPLANE * CUBEHALFSIZETOENCAPSULATINGSPHERERADIUS));
	}


	myContext->PSSetShader(*myShader, nullptr, 0);
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
			filtered = aScene->Cull(myCameras[i], preCull, false);
		}

		Render(myCameras[i], filtered, aBoneMapping);
	}
}

void DepthRenderer::RenderSpotLightDepth(SpotLight* aSpotlight, Scene* aScene, std::unordered_map<ModelInstance*, short>& aBoneMapping)
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
	myContext->PSSetShader(*myShader, nullptr, 0);
	myContext->RSSetViewports(1, &port);
	myContext->OMSetRenderTargets(1, &myRenderTarget1x6, myDepth1x6);

	std::vector<class ModelInstance*> filtered;
	{
		PERFORMANCETAG("Culling");
		filtered = aScene->Cull(aSpotlight->myCamera, false);
	}

	Render(aSpotlight->myCamera, filtered, aBoneMapping);
	myLastDepthView = myDepthsResource1x6;
}

void DepthRenderer::RenderEnvironmentDepth(EnvironmentLight* aLight, Scene* aScene, std::unordered_map<ModelInstance*, short>& aBoneMapping, std::function<void()> aPreRenderFunction)
{
	if (!aLight)
	{
		ONETIMEWARNING("Rendering without an envinronmentlight", "");
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

	DebugDrawer::GetInstance().DrawFrustum(myEnvironmentCamera->GenerateFrustum());

	std::vector<class ModelInstance*> filtered;
	{
		PERFORMANCETAG("Culling");
		filtered = aScene->Cull(myEnvironmentCamera, false);
	}


	if (aPreRenderFunction)
	{
		aPreRenderFunction();
	}
	myContext->PSSetShader(*myShader, nullptr, 0);
	Render(myEnvironmentCamera, filtered, aBoneMapping);
	myLastDepthView = myDepthsResource1x1;
}

ID3D11ShaderResourceView* DepthRenderer::RenderDecalDepth(Decal* aDecal, Scene* aScene, std::unordered_map<ModelInstance*, short>& aBoneMapping)
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
		SYSERROR("Could not shadow depth texture", "");
		return nullptr;
	}


	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	WIPE(dsvDesc);
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;

	result = myDevice->CreateDepthStencilView(tex, &dsvDesc, &depth);
	if (FAILED(result))
	{
		SYSERROR("Could not shadow depth view", "");
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
		SYSERROR("Could not shadow depth resource view", "");
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
	myContext->PSSetShader(*myShader, nullptr, 0);
	myContext->RSSetViewports(1, &port);
	myContext->OMSetRenderTargets(1, &myDecalRenderTarget, depth);

	std::vector<class ModelInstance*> filtered = aScene->Cull(aDecal->myCamera);
	Render(aDecal->myCamera, filtered, aBoneMapping);

	tex->Release();
	depth->Release();

	return view;
}

void DepthRenderer::Render(Camera* aCamera, const std::vector<ModelInstance*>& aModelList, std::unordered_map<ModelInstance*, short>& aBoneMapping)
{
	PERFORMANCETAG("DepthRender");
	Model* model = nullptr;
	Model::CModelData* modelData = nullptr;


	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE bufferData;


	FrameBufferData fData;
	WIPE(fData);
	fData.myWorldToCamera = CommonUtilities::Matrix4x4<float>::Transpose(CommonUtilities::Matrix4x4<float>::GetFastInverse(aCamera->GetTransform()));
	fData.myCameraToProjection = CommonUtilities::Matrix4x4<float>::Transpose(aCamera->GetProjection(false));
	fData.myCameraPosition = aCamera->GetPosition();
	fData.myTotalTime = RenderManager::GetTotalTime();



	WIPE(bufferData);
	result = myContext->Map(myFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
	if (FAILED(result))
	{
		SYSERROR("Could not map frame buffer", "");
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

		ObjectBufferData oData;
		Model::LodLevel* lodlevel;
		for (size_t modelIndex = 0; modelIndex < aModelList.size(); modelIndex++)
		{
			model = aModelList[modelIndex]->GetModel();
			if (!model->ShouldRender() || aModelList[modelIndex]->GetIsUsingSecondaryFov() || !aModelList[modelIndex]->GetCastsShadows())
			{
				continue;
			}
			modelData = model->GetModelData();

			if (modelData->myForceForward)
			{
				continue;
			}


			oData.myModelToWorldSpace = CommonUtilities::Matrix4x4<float>::Transpose(aModelList[modelIndex]->GetModelToWorldTransform());
			oData.myTint = aModelList[modelIndex]->GetTint();
			if (modelData->myshaderTypeFlags & ShaderFlags::HasBones)
			{
				oData.myBoneOffsetIndex = aBoneMapping[aModelList[modelIndex]];
			}

			oData.myObjectLifeTime = now - aModelList[modelIndex]->GetSpawnTime();
			oData.myObjectExpectedLifeTime = aModelList[modelIndex]->GetExpectedLifeTime();


			WIPE(bufferData);
			result = myContext->Map(myObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);

			if (FAILED(result))
			{
				SYSERROR("Could not map object buffer", "");
				return;
			}

			memcpy(bufferData.pData, &oData, sizeof(oData));
			myContext->Unmap(myObjectBuffer, 0);


			myContext->IASetPrimitiveTopology(modelData->myPrimitiveTopology);
			myContext->IASetInputLayout(modelData->myInputLayout);

			myContext->VSSetConstantBuffers(1, 1, &myObjectBuffer);
			myContext->VSSetShader(*modelData->myVertexShader, nullptr, 0);

			myContext->PSSetConstantBuffers(1, 1, &myObjectBuffer);
			if (modelData->myTextures[0]) { myContext->PSSetShaderResources(0, 1, *modelData->myTextures[0]); myContext->VSSetShaderResources(0, 1, *modelData->myTextures[0]); }
			if (modelData->myTextures[1]) { myContext->PSSetShaderResources(1, 1, *modelData->myTextures[1]); myContext->VSSetShaderResources(1, 1, *modelData->myTextures[1]); }
			if (modelData->myTextures[2]) { myContext->PSSetShaderResources(2, 1, *modelData->myTextures[2]); myContext->VSSetShaderResources(2, 1, *modelData->myTextures[2]); }

			lodlevel = model->GetOptimalLodLevel(aModelList[modelIndex]->GetPosition().DistanceSqr(aCamera->GetPosition()));
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
}
