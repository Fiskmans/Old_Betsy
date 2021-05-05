#pragma once
class Event
{
public:
	Event();
	~Event();

	virtual void Init() = 0;
	virtual void Update(float aDeltaTime) = 0;

	virtual void StartEvent() = 0;
	virtual void IdleEvent() = 0;
	virtual void EndEvent() = 0;
	virtual void CleanUpEvent() = 0;
private:

};

