#pragma once
#include "BaseState.h"
#include "Observer.hpp"
#include "Button.h"

class MenuSlider;

class PauseState : public BaseState , public Observer
{
public:
	PauseState(bool aShouldDeleteOnPop = true);
	~PauseState();

	void Update(const float aDeltaTime) override;
	bool Init();
	void Render(CGraphicsEngine* aGraphicsEngine) override;

	virtual void RecieveMessage(const Message& aMessage) override;

	void Activate() override;
	void Deactivate() override;

	void Unload() override;

private:
	int selectionIndex = 0;

	Button myResumeButton;
	Button myMainMenuButton;
	SpriteInstance* myFadeBackgroundSprite;
	SpriteInstance* myFadeSprite;
	SpriteInstance* myMousePointer;
	MenuSlider* myMasterVolumeSlider;
};

