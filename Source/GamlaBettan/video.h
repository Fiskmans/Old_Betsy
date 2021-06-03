#pragma once


struct ID3D11Texture2D;

struct ID3D11ShaderResourceView;
struct ID3D11Device;
struct ID3D11Texture2D;
struct ID3D11DeviceContext;
class VideoPlayer;
class SpriteInstance;
class SpriteFactory;



enum VideoStatus
{
	VideoStatus_Idle,
	VideoStatus_Playing,
	VideoStatus_ReachedEnd
};

class Video
{
public:

	Video();
	~Video();

	bool Init(const char* aPath, ID3D11Device* aDevice, SpriteFactory* aSpriteFactory);
	void Play(bool aLoop = false);
	void Pause();
	void Stop();
	void Restart();
	void Destroy();

	/* Will return false if there was something wrong with the load of the video */
	//bool Render();
	void Update(ID3D11DeviceContext* aDeviceContext,float aDeltatime);
		
	CommonUtilities::Vector2<int> GetVideoSize() const { return mySize; }
		
	bool HasVideoEnded();
	SpriteInstance* GetSpriteInstance();
private:
		
	VideoPlayer* myPlayer;
	SpriteInstance* myVideoSprite;
	ID3D11Texture2D* myVideoTexture;

	CommonUtilities::Vector2<int> mySize;
		
	VideoStatus myStatus;

	float myUpdateTime;
		
	bool myWantsToPlay;
	bool myIsLooping;
};