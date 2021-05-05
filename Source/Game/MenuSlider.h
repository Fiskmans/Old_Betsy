#pragma once
#include "Observer.hpp"
#include "Publisher.hpp"

class SpriteInstance;
class SpriteFactory;
class Scene;

class MenuSlider : public Observer, public Publisher
{
public:
	MenuSlider();
	~MenuSlider();

	void Init(SpriteFactory* aSpriteFactory, const V2F& aPos, const int& aMaxValue, MessageType aMessageToSendNewValue = MessageType::None, const int& aStartValue = 0);

	void AddToScene(Scene* aScene);
	void RemoveFromScene(Scene* aScene);

	void SetCurrentValue(int aValue);

	SpriteInstance* GetBackgroundSprite();
	SpriteInstance* GetSliderSprite();
	SpriteInstance* GetForegroundSprite();

	virtual void RecieveMessage(const Message& aMessage) override;

private:
	SpriteInstance* myBackground;
	SpriteInstance* mySlider;
	SpriteInstance* myForeground;

	V2F myPos;
	int myMaxValue;
	int myCurrentValue;

	V2F myHitboxMin;
	V2F myHitboxMax;

	MessageType myMessageType;

	SpriteFactory* mySpriteFactory;

	void SetPivot(const V2F& aPivot);
};