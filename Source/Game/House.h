#pragma once
#include "Component.h"
#include "AcceptInteraction.h"

class Scene;
class TextFactory;

class House : public Component, public Observer
{
public:
	virtual void InteractWith(Item* aHeldItem) override;

	virtual void RecieveMessage(const Message& aMessage) override;

	// Inherited via Component
	virtual void Init(Entity* aEntity) override;
	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnKillMe() override;

	void SetScenePtr(Scene* aScene);
	void SetTextFactory(TextFactory* aTextFactory);

private:
	AcceptInteraction myAcceptInteraction;
	int myTimeOfDay;
};

