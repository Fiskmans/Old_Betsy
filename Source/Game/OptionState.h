#pragma once
#include "BaseState.h"
#include "Button.h"
#include "Observer.hpp"
#include <array>
#include "Publisher.hpp"

class DirectX11Framework;
class AudioManager;
class Video;
class SpriteRenderer;

class OptionState : public BaseState, public Publisher, public Observer
{
public:
	OptionState(bool aShouldDeleteOnPop = true);
	~OptionState();
	virtual void Update(const float aDeltaTime) override;
	virtual void RecieveMessage(const Message& aMessage) override;
	bool Init(SpriteFactory* aSpritefactory, DirectX11Framework* aFramework, SpriteRenderer* aSpriteRenderer, Video* aVideoToKeepPlaying, SpriteInstance* aMousePointer);
	virtual void Render(CGraphicsEngine* aGraphicsEngine) override;

	virtual void Activate()override;
	virtual void Deactivate()override;

	virtual void Unload()override;

private:
	void InitLayout(SpriteFactory* aSpritefactory);

	struct StateInitData
	{
		SpriteFactory* mySpriteFactory;
		DirectX11Framework* myFrameWork;
		SpriteRenderer* mySpriteRenderer;
	} myStateInitData;

	Button myBackButton;

	std::array<Button, 4> myScreensizeButtons;

	SpriteInstance* myMousePointerPtr;

	Video* myVideoPtr;

	bool myIsActive = false;
};