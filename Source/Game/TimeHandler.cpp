#include "pch.h"
#include "TimeHandler.h"
#include "TraderEvent.h"
#include "AnimationEvent.h"
#include "TemporaryItemEvent.h"

#include "Random.h"
#include "Entity.h"
#include "ObjectPool.hpp"

#include "AssetManager.h"

TimeHandler::TimeHandler() :
	myCurrentHour(6),
	myCurrentMinute(0),
	mySeconds(0.0f)
{
}

TimeHandler::~TimeHandler()
{
}

void TimeHandler::Init()
{
	LoadAnimationEventsFromFile();
	LoadSpecialDaysFromFile();

	myCurrentDay = 1;
	myDayTimePercentage = 0;

	if (mySpecialDays.count(myCurrentDay) == 0)
	{
		myToday = CreateRandomDay();
	}
	else
	{
		myToday = mySpecialDays[myCurrentDay];
	}

	mySavedTimeModifier = myTimeModifier;
}

void TimeHandler::Update(float aDeltaTime)
{
	mySeconds += aDeltaTime * myTimeModifier;
	//myDayTimePercentage = (myCurrentHour * 3600.0f + myCurrentMinute * 60.0f + mySeconds) / 86400.0f;
	//SCALED TO FIT 06-24
	myDayTimePercentage = ((myCurrentHour * 3600.0f + myCurrentMinute * 60.0f + mySeconds) - 21600) / 64800.0f;
	if (mySeconds > 60.0f)
	{
		mySeconds -= 60.0f;
		myCurrentMinute++;

		Message updateTimeMessage;
		updateTimeMessage.myMessageType = MessageType::UpdateTime;
		updateTimeMessage.myFloatValue = myDayTimePercentage;
		updateTimeMessage.myIntValue = myCurrentHour;
		updateTimeMessage.myIntValue2 = myCurrentDay;
		PostMaster::GetInstance()->SendMessages(updateTimeMessage);

		if (myCurrentMinute >= 60)
		{
			myCurrentMinute = 0;
			myCurrentHour++;

			if (myCurrentDay == 14 && myCurrentHour == 12)
			{
				Message message;
				message.myMessageType = MessageType::SendUIGameMessage;
				message.myText = "This is the last day before winter! I better store everything I can";
				PostMaster::GetInstance()->SendMessages(message);
			}

			if (myCurrentHour == 23)
			{
				Message message;
				message.myMessageType = MessageType::SendUIGameMessage;
				message.myText = "I'm getting sleepy";
				PostMaster::GetInstance()->SendMessages(message);
			}

			if (myCurrentHour >= 24)
			{
				NextDay();
				SYSINFO("NEW DAY");
			}

			if (myToday.myEvents[myCurrentHour] != nullptr)
			{
				myToday.myEvents[myCurrentHour]->StartEvent();
				SYSINFO("New hour, event started!!");
			}
			else
			{
				SYSINFO("New hour, no event occured");
			}
		}
	}

	for (int hour = 6; hour < 24; hour++)
	{
		if (myToday.myEvents[hour] != nullptr)
		{
			myToday.myEvents[hour]->Update(aDeltaTime);
		}
	}
}

const int TimeHandler::GetCurrentDay()
{
	return myCurrentDay;
}

const int TimeHandler::GetCurrentHour()
{
	return myCurrentHour;
}

const float TimeHandler::GetDayTimePercentage()
{
	return myDayTimePercentage;
}

void TimeHandler::NextDay()
{

	for (int index = 6; index < 24; index++)
	{
		if (myToday.myEvents[index] != nullptr)
		{
			myToday.myEvents[index]->CleanUpEvent();
		}
	}

	myCurrentDay++;
	myCurrentHour = 6;
	myCurrentMinute = 0;
	mySeconds = 0.0f;

	if (mySpecialDays.count(myCurrentDay) == 0)
	{
		myToday = CreateRandomDay();
	}
	else
	{
		myToday = mySpecialDays[myCurrentDay];
	}

	PostMaster::GetInstance()->SendMessages(MessageType::DespawnTrader);

	if (myCurrentDay >= 15)
	{
		Message endGameMessage;
		endGameMessage.myMessageType = MessageType::EndGame;
		PostMaster::GetInstance()->SendMessages(endGameMessage);
		PauseTime();
		return;
	}
	else
	{
		PostMaster::GetInstance()->SendMessages(MessageType::GoToSleep);
	}

	PostMaster::GetInstance()->SendMessages(MessageType::CreateSeed);
}

void TimeHandler::AddEventToDay(Event* aEvent, int aDayIndex, int aHourIndex)
{
	if (mySpecialDays.count(aDayIndex) > 0)
	{
		mySpecialDays[aDayIndex].myEvents[aHourIndex] = aEvent;
	}
	else
	{
		mySpecialDays[aDayIndex] = CreateRandomDay();
	}
}

void TimeHandler::LoadSpecialDaysFromFile()
{
	FiskJSON::Object& root = AssetManager::GetInstance().GetJSON("events/Animation.json").GetAsJSON();

	for (auto& i : root["days"].Get<FiskJSON::Array>())
	{
		int index;
		Day specialDay;

		(*i)["dayindex"].GetIf(index);

		for (auto& inEvent : (*i)["events"].Get<FiskJSON::Array>())
		{
			Event* event = nullptr;
			int specificID;
			int type = (*inEvent)["typeofevent"].Get<int>();
			switch (type)
			{
			case 0:
				specificID = (*inEvent)["specificID"].Get<int>();
				if (specificID == 0)
				{
					event = CreateRandomAnimationEvent();
				}
				else
				{
					event = CreateSpecificAnimationEvent(specificID);
				}

				break;
			case 1:
				event = new TraderEvent();
				break;
			case 2:
				event = new TemporaryItemEvent();
				break;
			case 3:
				//event = new DialogueEvent();
				break;
			default:
				SYSERROR("Error in specialdays json, an event has unused typeofevent id", std::to_string(static_cast<int>(type)));
				break;
			}

			if (event)
			{
				specialDay.myEvents[(*inEvent)["time"].Get<int>()] = event;
			}
			else
			{
				SYSERROR("Error in LoadSpecialDaysFromFile, created event is null");
			}
		}

		mySpecialDays[index] = specialDay;
	}
}

void TimeHandler::LoadAnimationEventsFromFile()
{
	FiskJSON::Object& root = AssetManager::GetInstance().GetJSON("events/Animation.json").GetAsJSON();

	for (auto& i : root["AnimationEvents"].Get<FiskJSON::Array>())
	{
		bool success = true;

		AnimationEventData data;
		success &= (*i)["name"].GetIf(data.name);
		success &= (*i)["fbxPath"].GetIf(data.fbxPath);
		success &= (*i)["spawnDuration"].GetIf(data.spawnDuration);
		success &= (*i)["loopDuration"].GetIf(data.loopDuration);
		success &= (*i)["endDuration"].GetIf(data.endDuration);
		success &= (*i)["xPos"].GetIf(data.posX);
		success &= (*i)["yPos"].GetIf(data.posY);
		success &= (*i)["zPos"].GetIf(data.posZ);

		if (!success)
		{
			SYSERROR("Error loading animation event", data.name);
		}


		myLoadedAnimationEvents.push_back(data);
	}
}

Day TimeHandler::CreateRandomDay()
{
	Day day;

	for (int index = 0; index < 10; index++)
	{
		int time = Tools::RandomRange(6, 23);
		if (day.myEvents[time] == nullptr)
		{
			day.myEvents[time] = CreateRandomAnimationEvent();
		}
	}
	return day;
}

Event* TimeHandler::CreateRandomAnimationEvent()
{
	int rand = Tools::RandomRange(0, CAST(int, myLoadedAnimationEvents.size() - 1));

	Event* event = new AnimationEvent();
	event->Init();
	V3F pos = V3F(myLoadedAnimationEvents[rand].posX, myLoadedAnimationEvents[rand].posY, myLoadedAnimationEvents[rand].posZ);
	CAST(AnimationEvent*, event)->SetUp(myLoadedAnimationEvents[rand].fbxPath, myLoadedAnimationEvents[rand].spawnDuration, myLoadedAnimationEvents[rand].loopDuration, myLoadedAnimationEvents[rand].endDuration, pos, myAnimationEntityVectorPtr, myEntityPool);
	return event;
}

Event* TimeHandler::CreateSpecificAnimationEvent(int aEventID)
{
	Event* event = new AnimationEvent();
	event->Init();
	V3F pos = V3F(myLoadedAnimationEvents[aEventID].posX, myLoadedAnimationEvents[aEventID].posY, myLoadedAnimationEvents[aEventID].posZ);
	CAST(AnimationEvent*, event)->SetUp(myLoadedAnimationEvents[aEventID].fbxPath, myLoadedAnimationEvents[aEventID].spawnDuration, myLoadedAnimationEvents[aEventID].loopDuration, myLoadedAnimationEvents[aEventID].endDuration, pos, myAnimationEntityVectorPtr, myEntityPool);
	return event;
}

void TimeHandler::PauseTime()
{
	myTimeModifier = 0.0f;
}

void TimeHandler::ResumeTime()
{
	myTimeModifier = mySavedTimeModifier;
}

void TimeHandler::SetEntityVectors(std::vector<Entity*>* aAnimationEntityVector, std::vector<Entity*>* aCharacterEntityVector, CommonUtilities::ObjectPool<Entity>* aEntityPool)
{
	myEntityVectorPtr = aCharacterEntityVector;
	myAnimationEntityVectorPtr = aAnimationEntityVector;
	myEntityPool = aEntityPool;
}
