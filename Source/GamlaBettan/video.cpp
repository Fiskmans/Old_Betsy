#include "pch.h"
#include "Sprite.h"
#include "video.h"
#include "videoplayer.h"
#include "GraphicEngine.h"
#include <d3d11_1.h>
#include "SpriteInstance.h"
#include "SpriteFactory.h"
#include "AssetManager.h"

Video::Video() : myPlayer(nullptr)
 {
	myUpdateTime = 0.0f;
	myStatus = VideoStatus_Idle;
	myWantsToPlay = false;
	myIsLooping = false;
	myVideoTexture = nullptr;
	myVideoSprite = nullptr;
}

Video::~Video()
{
	Destroy();
}

void Video::Play(bool aLoop)
{
	myWantsToPlay = true;
	myIsLooping = aLoop;
}

void Video::Pause()
{
	myWantsToPlay = false;
}

void Video::Stop()
{
	myWantsToPlay = false;
	myPlayer->Stop();
}

void Video::Restart()
{
	myWantsToPlay = true;
	myPlayer->RestartStream();
}

void Video::Destroy()
{
	if (myPlayer != nullptr)
	{
		myPlayer->~VideoPlayer();
		delete myPlayer;
		myPlayer = nullptr;
	}

	if (myVideoSprite != nullptr)
	{
		delete myVideoSprite;
		myVideoSprite = nullptr;
	}
	if (myPlayer != nullptr)
	{
		myPlayer->Stop();
		myPlayer->~VideoPlayer();
		delete myPlayer;
		myPlayer = nullptr;
	}
}

bool Video::HasVideoEnded()
{
	if (myPlayer->GetIsDone() == true)
	{
		return true;

	}
	return false;
}



bool Video::Init(const char* aPath, ID3D11Device* aDevice, SpriteFactory* aSpriteFactory)
{
	if (myPlayer != nullptr)
	{
		return false;
	}

	myPlayer = new VideoPlayer();
	if (!myPlayer->Init(aPath))
	{
		return false;
	}

	if (!myPlayer->DoFirstFrame())
	{
		return false;
	}

	myVideoSprite = aSpriteFactory->CreateVideoSprite();
	mySize.x = myPlayer->l_pAVFrame->width;
	mySize.y = myPlayer->l_pAVFrame->height;

	myStatus = VideoStatus_Playing;

	D3D11_TEXTURE2D_DESC texture_desc;
	texture_desc.Width = mySize.x;
	texture_desc.Height = mySize.y;
	texture_desc.MipLevels = 1;
	texture_desc.ArraySize = 1;
	texture_desc.MipLevels = 1;
	texture_desc.ArraySize = 1;

	texture_desc.SampleDesc.Count = 1;
	texture_desc.SampleDesc.Quality = 0;
	texture_desc.Usage = D3D11_USAGE_DYNAMIC;
	texture_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	texture_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texture_desc.MiscFlags = 0;

	aDevice->CreateTexture2D(&texture_desc, nullptr, &myVideoTexture);
	ID3D11ShaderResourceView* view;
	aDevice->CreateShaderResourceView(myVideoTexture, NULL, &view);

	Asset* textureAsset = new TextureAsset(view);

	myVideoSprite->GetSprite()->GetSpriteData().myTexture = AssetHandle(textureAsset);

	AssetManager::GetInstance().AssumeOwnershipOfCustomAsset(textureAsset);

	return true;
}

SpriteInstance* Video::GetSpriteInstance()
{
	return myVideoSprite;
}
void Video::Update(ID3D11DeviceContext* aDeviceContext,float aDeltatime)
{
	if (myWantsToPlay == true)
	{
		double videoFPS = myPlayer->GetFps();

		double timeLeft = myPlayer->myLeftOverTime;
		timeLeft += aDeltatime;

		unsigned int fpsCount = 0;
		while (timeLeft > 1 / videoFPS)
		{
			++fpsCount;
			timeLeft -= 1 / videoFPS;
		}

		myPlayer->myLeftOverTime = timeLeft;

		for (unsigned int i = 0; i < fpsCount; ++i)
		{
			int status = myPlayer->GrabNextFrame();
			if (status < 0)
			{
				if (myIsLooping == true)
				{
					myPlayer->RestartStream();
				}
				else
				{
					myPlayer->SetIsDone(true);
				}
			}
		}

		if (fpsCount > 0)
		{
			D3D11_MAPPED_SUBRESOURCE  mappedResource;
			HRESULT result = aDeviceContext->Map(myVideoTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			if (FAILED(result))
			{
				//error thingy
			}

			int* source = (int*)(mappedResource.pData);

			myPlayer->Update(source);

			aDeviceContext->Unmap(myVideoTexture, 0);
		}

	}
}
