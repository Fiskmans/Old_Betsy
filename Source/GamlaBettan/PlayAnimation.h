#pragma once
#include "CinematicEvent.h"
#include "CinematicTarget.h"
class PlayAnimation : public CinematicEvent
{
public:
	PlayAnimation(Scene* aScene);
	PlayAnimation(FiskJSON::Object& aObject, Scene* aScene);
	// Inherited via CinematicEvent
	virtual void Update(float aTime) override;
	virtual void AppendTo(FiskJSON::Object& aObject) override;
	virtual void Edit() override;
	virtual const char* GetType() override;

private:
	bool myIsplaying;
	CinematicTarget myTarget;
	int myAnimation;
};

