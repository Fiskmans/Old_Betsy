#pragma once
#include "CommonUtilities\Singleton.hpp"
#include "Event.h"
#include "CommonUtilities\ObjectPool.hpp"

class Entity;

struct Day
{
	std::array<Event*, 24> myEvents = { nullptr };
};

struct AnimationEventData
{
	std::string name = "Event name not loaded";
	std::string fbxPath;
	float spawnDuration;
	float loopDuration;
	float endDuration;
	float posX;
	float posY;
	float posZ;
};

class TimeHandler : public CommonUtilities::Singleton<TimeHandler>
{
	friend class  CommonUtilities::Singleton<TimeHandler>;
public:

	TimeHandler();
	~TimeHandler();

	void Init();
	void Update(float aDeltaTime);
	const int GetCurrentDay();
	const int GetCurrentHour();
	const float GetDayTimePercentage();
	void NextDay();
	void AddEventToDay(Event* aEvent, int aDayIndex, int aHourIndex);
	Event* CreateRandomAnimationEvent();
	Event* CreateSpecificAnimationEvent(int aEventID);
	void PauseTime();
	void ResumeTime();
	void SetEntityVectors(std::vector<Entity*>* aAnimationEntityVector, std::vector<Entity*>* aCharacterEntityVector, CommonUtilities::ObjectPool<Entity>* aEntityPool);

private:
	void LoadSpecialDaysFromFile();
	void LoadAnimationEventsFromFile();
	Day CreateRandomDay();
	
	Day myToday;
	int myCurrentDay;
	int myCurrentHour;
	int myCurrentMinute;
	float mySeconds;
	float myDayTimePercentage;

	float myTimeModifier = 240.0f;
	float mySavedTimeModifier;
	std::unordered_map<int, Day> mySpecialDays;
	std::vector<AnimationEventData> myLoadedAnimationEvents;
	std::vector<Entity*>* myEntityVectorPtr = nullptr;
	std::vector<Entity*>* myAnimationEntityVectorPtr = nullptr;
	CommonUtilities::ObjectPool<Entity>* myEntityPool = nullptr;
};