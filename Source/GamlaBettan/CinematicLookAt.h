#pragma once
#include "CinematicEvent.h"
#include "CinematicTarget.h"

class CinematicLookAt : public CinematicEvent
{
public:
	CinematicLookAt(CScene* aScene);
	CinematicLookAt(FiskJSON::Object& aObject, CScene* aScene);
	// Inherited via CinematicEvent
	virtual void Update(float aTime) override;
	virtual void AppendTo(FiskJSON::Object& aObject) override;
	virtual void Edit() override;
	virtual const char* GetType() override;


private:
	CinematicTarget myTarget;
	V3F myTargetPosition;
};

