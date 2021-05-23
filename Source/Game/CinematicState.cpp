#include "pch.h"
#include "CinematicState.h"
#include <TimeHelper.h>
#include <GraphicEngine.h>
#include <Scene.h>
#include <Camera.h>
#include <fstream>
#include <SpriteFactory.h>
#include "SpriteInstance.h"

#include <CameraPan.h>
#include <ParticleBurst.h>
#include <FadeScreen.h>
#include <MoveObject.h>
#include <PlayAnimation.h>


#include <FiskJSON.h>


bool CinematicState::Init(const std::string& aManuscript, Scene* aScene, ParticleFactory* aParticleFactory, SpriteFactory* aSpriteFactory)
{
	myScene = aScene;
	myParticleFactory = aParticleFactory;
	myCameaSpeed = 800.f;
	myFadeSprite = aSpriteFactory->CreateSprite("engine/fadeScreen.dds");
	myFadeSprite->SetScale(V2F(1000.f, 1000.f));
	myFadeSprite->SetColor(V4F(1, 1, 1, 0));
	myFadeSprite->SetPosition(V2F(0, 0));

	if (!Load(aManuscript))
	{
		return false;
	}


	return true;
}

void CinematicState::Update(const float aDeltaTime)
{
	for (auto& i : *myScene)
	{
		if (i->HasAnimations())
		{
			ONETIMEWARNING("Cinematic animations are currently broken", "");
			//i->StepAnimation(aDeltaTime);
		}
	}
	switch (myState)
	{
	case CinematicState::State::Setup:
	{
		V3F cameraAt = myScene->GetMainCamera()->GetPosition();
		V3F delta = myCameraStartPos - cameraAt;
		float distance = myCameaSpeed * aDeltaTime;
		if (delta.LengthSqr() < distance * distance)
		{
			myStartTime = Tools::GetTotalTime();
			myState = State::Running;
		}
		else
		{
			myScene->GetMainCamera()->SetPosition(cameraAt + delta.GetNormalized() * distance);
		}
	}
	break;
	case CinematicState::State::Running:
	{
		float now = Tools::GetTotalTime();
		float cinemticTime = now - myStartTime;
		for (auto& ev : myEvents)
		{
			ev->Update(cinemticTime);
		}
		if (cinemticTime > myTotalTime)
		{
			myState = State::Cleanup;
		}
	}
	break;
	case CinematicState::State::Cleanup:
	{
		V3F cameraAt = myScene->GetMainCamera()->GetPosition();
		V3F delta = myCameraEndPos - cameraAt;
		float distance = myCameaSpeed * aDeltaTime;
		if (delta.LengthSqr() < distance * distance)
		{
			myState = State::Setup;					//popstate

			PostMaster::GetInstance()->SendMessages(MessageType::PopState);

		}
		else
		{
			myScene->GetMainCamera()->SetPosition(cameraAt + delta.GetNormalized() * distance);
		}
	}
	break;
	default:
		throw "This is wrong";
		break;
	}
	myScene->Update(aDeltaTime);
}

void CinematicState::Render(CGraphicsEngine* aGraphicsEngine)
{
	aGraphicsEngine->RenderFrame(myScene);
}

void CinematicState::Activate()
{
	myCameraEndPos = myScene->GetMainCamera()->GetPosition();
	myState = State::Setup;
	myScene->AddSprite(myFadeSprite);
}

void CinematicState::Deactivate()
{
	myScene->RemoveSprite(myFadeSprite);
}

void CinematicState::AddEvent(CinematicEvent* aEvent)
{
	aEvent->SetData(myScene, myScene->GetMainCamera(), myParticleFactory, myFadeSprite);
	myEvents.push_back(aEvent);
}

bool CinematicState::Load(const std::string& aManuscript)
{
	for (auto& i : myEvents)
	{
		SAFE_DELETE(i);
	}
	myEvents.clear();
	std::ifstream inFile;
	inFile.open(aManuscript);
	if (!inFile)
	{
		return false;
	}
	std::string inData((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());

	FiskJSON::Object root;
	try
	{
		root.Parse(inData);
		LOGVERBOSE(root.Serialize(true));
	}
	catch (const FiskJSON::Invalid_JSON & e)
	{
		LOGERROR("Cinematic json could not parse: [" + std::string(e.what()) + "]");
	}

	root["CameraStart"].GetIf(myCameraStartPos);


	V3F cameraAt = myCameraStartPos;
	std::string type;
	CinematicEvent* parsed = nullptr;
	float endTime = 0.f;
	for (auto& JSONevent : root["Events"].Get<FiskJSON::Array>())
	{
		parsed = nullptr;
		if ((*JSONevent)["Type"].GetIf(type))
		{
			if (type == "cameraPan")
			{
				LOGINFO("camerapan");
				CinematicPath path;
				path.ParseFrom((*JSONevent)["Path"]);
				path.SetStart(cameraAt);
				cameraAt = path.Evaluate(1.f);
				parsed = new CameraPan(path);
			}
			else if (type == "particleBurst")
			{
				LOGINFO("particleBurst");
				std::string path;
				V3F position;
				V3F direction;
				(*JSONevent)["Direction"].GetIf(direction);
				float area;
				if ((*JSONevent)["Position"].GetIf(position) && (*JSONevent)["Area"].GetIf(area) && (*JSONevent)["Path"].GetIf(path))
				{
					ParticleBurst* burst = new ParticleBurst(path, position, area, direction);
					parsed = burst;
				}
			}
			else if (type == "idle")
			{
				LOGINFO("idle");
				float start;
				float duration;
				if (JSONevent->GetIf(start) && JSONevent->GetIf(duration))
				{
					float lastsUntil = start + duration;
					if (lastsUntil > endTime)
					{
						endTime = lastsUntil;
					}
				}
			}
			else if (type == "SceenFade")
			{
				bool out;
				if ((*JSONevent)["Fade"].GetIf(out))
				{
					parsed = new FadeScreen(out);
				}
			}
			else if (type == "MoveObject")
			{
				parsed = new MoveObject(*JSONevent, myScene);
			}
			else if (type == "Play Animation")
			{
				parsed = new PlayAnimation(*JSONevent, myScene);
			}
			/*else if (type == "Look At")
			{
				parsed = new CinematicLookAt(*JSONevent, myScene);
			}*/
		}
		if (parsed)
		{
			float start;
			float duration;
			if ((*JSONevent)["Start"].GetIf(start) && (*JSONevent)["Duration"].GetIf(duration))
			{
				parsed->SetTiming(start, duration);
				float lastsUntil = start + duration;
				if (lastsUntil > endTime)
				{
					endTime = lastsUntil;
				}
				std::string customName;
				if ((*JSONevent)["CustomName"].GetIf(customName))
				{
					parsed->SetName(customName);
				}
				AddEvent(parsed);
			}
		}
	}
	myTotalTime = endTime;
}
