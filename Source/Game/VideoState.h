#pragma once
#include "Publisher.hpp"
#include "BaseState.h"
#include "Observer.hpp"
class Video;
struct ID3D11DeviceContext;

class VideoState : public BaseState, public Publisher, public Observer
{
public:

	enum class VideoType
	{
		intro,
		cutscene,
		outro,
		none
	};

	VideoState(bool aShouldDeleteOnPop = true);
	~VideoState();
	virtual void Update(const float aDeltaTime) override;
	virtual void RecieveMessage(const Message& aMessage) override;
	bool Init(ModelLoader* aModelLoader, SpriteFactory* aSpritefactory, const char* aPath, bool aShouldLoop, ID3D11DeviceContext* aDeviceContext);

	virtual void Render(CGraphicsEngine* aGraphicsEngine) override;
	virtual void Activate()override;
	virtual void Deactivate()override;

	virtual void Unload()override;

	void SetVideoType(VideoType aType);

private:
	void StartVideo();
	void Destroy(bool aShouldPop);
	struct VideoInitData
	{
		ID3D11DeviceContext* myDeviceContex;
		ModelLoader* myModelLoader;
		SpriteFactory* mySpritefactory;
		const char* myPath;
		bool myShouldLoop;
	};
	VideoInitData myVideoInitData;
	Video* myVideo;
	VideoType myVideoType = VideoType::none;
};

