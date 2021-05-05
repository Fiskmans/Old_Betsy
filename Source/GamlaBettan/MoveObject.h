#pragma once
#include "CinematicEvent.h"
#include <Vector3.hpp>
#include "CinematicPath.h"
#include "CinematicTarget.h"
class ModelInstance;

class MoveObject :
	public CinematicEvent
{
public: 
	MoveObject(V3F aFrom, V3F aTo, Scene* aScene);
	MoveObject(CinematicPath aPath);
	MoveObject(FiskJSON::Object& aObject,Scene* aScene);


private:
	void SetModel(ModelInstance* aModel);

	CinematicPath myPath;
	CinematicTarget myTarget;

	// Inherited via CinematicEvent
	virtual void Update(float aTime) override;
	virtual void AppendTo(FiskJSON::Object& aObject) override;
	virtual void Edit() override;
	virtual const char* GetType() override;
};

