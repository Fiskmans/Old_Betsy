#pragma once
#include "Event.h"

class TemporaryItemEvent :
	public Event
{

public:

	TemporaryItemEvent();
	~TemporaryItemEvent();

	// Inherited via Event
	virtual void Init() override;
	virtual void Update(float aDeltaTime) override;
	virtual void StartEvent() override;
	virtual void IdleEvent() override;
	virtual void EndEvent() override;
	virtual void CleanUpEvent() override;

private:
};

