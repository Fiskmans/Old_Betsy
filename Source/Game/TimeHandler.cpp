#include "pch.h"
#include "TimeHandler.h"
#include "TraderEvent.h"
#include "AnimationEvent.h"
#include "TemporaryItemEvent.h"

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include "Random.h"
#include "Entity.h"
#include "ObjectPool.hpp"

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
	rapidjson::Document SpecialDaysDoc;

#pragma warning(suppress : 4996)
	FILE* fp = fopen("Data\\Metrics\\SpecialDays.json", "rb");
	char readBuffer[4096];
	rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	SpecialDaysDoc.ParseStream(is);
	fclose(fp);

	auto daysArray = SpecialDaysDoc["Days"].GetArray();

	for (int i = 0; i < daysArray.Size(); ++i)
	{
		int index = daysArray[i]["dayindex"].GetInt();
		Day specialDay;

		auto eventsArray = daysArray[i]["events"].GetArray();
		for (int eventIndex = 0; eventIndex < eventsArray.Size(); eventIndex++)
		{
			Event* event = nullptr;
			int specificID;
			switch (eventsArray[eventIndex]["typeofevent"].GetInt())
			{
			case 0:
				specificID = eventsArray[eventIndex]["specificID"].GetInt();
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
				SYSERROR("Error in specialdays json, an event has unused typeofevent id", "error in json");
				break;
			}

			if (event)
			{
				specialDay.myEvents[eventsArray[eventIndex]["time"].GetInt()] = event;
			}
			else
			{
				SYSERROR("Error in LoadSpecialDaysFromFile, created event is null", "BIG ERROR");
			}
		}

		mySpecialDays[index] = specialDay;
	}
}

void TimeHandler::LoadAnimationEventsFromFile()
{
	//FISKJSON LOAD EVENTS
	rapidjson::Document animationEventsDoc;

#pragma warning(suppress : 4996)
	FILE* fp = fopen("Data\\Metrics\\AnimationEvents.json", "rb");
	char readBuffer[4096];
	rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	animationEventsDoc.ParseStream(is);
	fclose(fp);

	auto animationEventsArray = animationEventsDoc["AnimationEvents"].GetArray();

	for (int i = 0; i < animationEventsArray.Size(); ++i)
	{
		AnimationEventData data;
		data.name = animationEventsArray[i]["name"].GetString();
		data.fbxPath = animationEventsArray[i]["fbxPath"].GetString();
		data.spawnDuration = animationEventsArray[i]["spawnDuration"].GetFloat();
		data.loopDuration = animationEventsArray[i]["loopDuration"].GetFloat();
		data.endDuration = animationEventsArray[i]["endDuration"].GetFloat();
		data.posX = animationEventsArray[i]["xPos"].GetFloat();
		data.posY = animationEventsArray[i]["yPos"].GetFloat();
		data.posZ = -animationEventsArray[i]["zPos"].GetFloat();
		myLoadedAnimationEvents[i + 1] = data;
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
