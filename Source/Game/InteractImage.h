#pragma once
#include "Component.h"
#include "Observer.hpp"

class SpriteFactory;
class Scene;
class SpriteInstance;

class InteractImage : public Component, public Observer
{
public:
	void PreInit(Scene* aScene,SpriteFactory* aFactory);
	void SetImagePath(const std::string& aFilePath);

	virtual void InteractWith(Item* aHeldItem) override;

	// Inherited via Component
	virtual void Init(Entity* aEntity) override;
	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;


private:

	void Activate();
	void Deactivate();

	bool myIsActive;

	std::string myFilePath;

	Scene* myScene;
	SpriteFactory* myFactory;
	SpriteInstance* mySprite;


	// Inherited via Observer
	virtual void RecieveMessage(const Message& aMessage) override;

};

