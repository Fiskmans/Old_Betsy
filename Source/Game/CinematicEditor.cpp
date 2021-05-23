#include <pch.h>
#include "CinematicEditor.h"
#include "WindowControl.h"
#include "ImGuiPackage.h"
#include "CinematicState.h"
#include "FiskJSON.h"
#include <fstream>
#include "CinematicEvent.h"
#include <CameraPan.h>
#include "Scene.h"
#include <ParticleBurst.h>
#include <FadeScreen.h>
#include <MoveObject.h>
#include <PlayAnimation.h>
#ifdef _DEBUG
#include "DebugTools.h"
#endif // _DEBUG


class SpriteFactory;

void CinematicEditor::Init(Scene* aScene,ParticleFactory* aPartFactory, SpriteFactory* aSpriteFactory)
{
	myScene = aScene;
	myState = new CinematicState();
	myState->Init("", aScene, aPartFactory, aSpriteFactory);
	bool fal = false;
	memcpy(reinterpret_cast<char*>(const_cast<bool*>(&myState->myShouldDeleteOnPop)),&fal,sizeof(bool));
}

void CinematicEditor::Init(CinematicState* aState)
{
	myState = aState;
	bool fal = false;
	memcpy(reinterpret_cast<char*>(const_cast<bool*>(&myState->myShouldDeleteOnPop)), &fal, sizeof(bool));
}

bool CinematicEditor::Run()
{
	return WindowControl::Window("Cinematic Editor", std::bind(&CinematicEditor::Imgui,this));
}

void CinematicEditor::Imgui()
{
#if USEIMGUI
	static char filename[128] = {};
	if (ImGui::Button("Save"))
	{
		Save();
	}
	ImGui::SameLine();
	if (ImGui::Button("Load"))
	{
		ImGui::OpenPopup("FileSelector");
	}
	ImGui::SameLine();
	if (ImGui::Button("New"))
	{
		ImGui::OpenPopup("NewFileDialogue");
	}
	ImGui::SameLine();
	if (ImGui::Button("Test"))
	{
		Test();
	}
	ImGui::Text("File: %s", myLoaded.c_str());

	if (ImGui::BeginPopup("NewFileDialogue"))
	{
		ImGui::InputText("Name", filename, 128);
		if (ImGui::Button("Select"))
		{
			std::string file = filename;
			file = "Data/Cinematics/" + file + ".cinm";
			Load(file);
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
	if (ImGui::BeginPopup("FileSelector"))
	{
#ifdef _DEBUG
		ImGui::Text("Files");
		ImGui::Separator();
		for (auto& i : (*DebugTools::FileList)[".cinm"])
		{
			if (ImGui::Selectable(i.c_str()))
			{
				Load(i);
			}
		}
		ImGui::EndPopup();
#endif
	}
	ImGui::Separator();
	if (ImGui::Button("Add event"))
	{
		ImGui::OpenPopup("TypeSelector");
	}
	if (ImGui::BeginPopup("TypeSelector"))
	{
		std::vector<std::pair<std::string, EventType>> types =
		{
			{"Camera pan",EventType::CameraPan},
			{"Particle burst",EventType::ParticleBurst},
			{"Screen fade",EventType::ScreenFade},
			{"Move object",EventType::MoveObject},
			{"Play Animation",EventType::PlayAnimation},
			{"Look at",EventType::LookAt}
		};

		ImGui::Text("Files");
		ImGui::Separator();
		for (auto& i : types)
		{
			if (ImGui::Selectable(i.first.c_str()))
			{
				AddEvent(i.second);
			}
		}
		ImGui::EndPopup();
	}

	Tools::EditPosition("Camera Start",myState->myCameraStartPos);

	ImGui::BeginChild("eventList");
	for (size_t index = 0; index < myState->myEvents.size(); index++)
	{
		CinematicEvent* i = myState->myEvents[index];
		ImGui::PushID(i);
		if (ImGui::TreeNode("Treenode","%s: %s",i->GetType(),i->GetName()))
		{
			if (ImGui::Button("^"))
			{
				if (index != 0)
				{
					std::swap(myState->myEvents[index], myState->myEvents[index - 1]);
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("v"))
			{
				if (index+1 != myState->myEvents.size())
				{
					std::swap(myState->myEvents[index], myState->myEvents[index + 1]);
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("X"))
			{
				ImGui::OpenPopup("Sure?");
			}
			if (ImGui::BeginPopup("Sure?"))
			{
				ImGui::Text("Are you sure?");
				if (ImGui::Button("Yes"))
				{
					myState->myEvents.erase(myState->myEvents.begin() + index);
					ImGui::CloseCurrentPopup();
					--index;
				}
				ImGui::SameLine();
				if (ImGui::Button("No"))
				{
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
			i->NameBox();

			ImGui::InputFloat("Duration", &i->myDuration);
			ImGui::DragFloat("Start at", &i->myStart,0.1f,0,myState->myTotalTime-i->myDuration);

			i->Edit();
			ImGui::Separator();
			ImGui::TreePop();
		}
		ImGui::PopID();
	}
	ImGui::EndChild();
#endif // USEIMGUI
}

void CinematicEditor::Load(const std::string& aFile)
{
	LOGINFO("Loading: " + aFile);
	myLoaded = aFile;
	myState->Load(aFile);
}

void CinematicEditor::Save()
{
	FiskJSON::Object root;
	FiskJSON::Object* startPosition = new FiskJSON::Object();
	startPosition->MakeArray();
	for (auto& i : myState->myCameraStartPos)
	{
		startPosition->PushValueChild(i);
	}
	root.AddChild("CameraStart", startPosition);
	FiskJSON::Object* events = new FiskJSON::Object();
	events->MakeArray();
	for (auto& ev : myState->myEvents)
	{
		ev->AppendTo(*events);
	}
	root.AddChild("Events", events);
	std::ofstream outFile;
	outFile.open(myLoaded);
	outFile << root.Serialize(true);
}

void CinematicEditor::AddEvent(CinematicEditor::EventType aType)
{
	CinematicEvent* ev = nullptr;
	switch (aType)
	{
	case CinematicEditor::EventType::CameraPan:
		ev = new CameraPan(V3F(0, 0, 0), V3F(0, 0, 0));
		break;
	case CinematicEditor::EventType::ParticleBurst:
		ev = new ParticleBurst("",V3F(0,0,0),0,V3F(0,0,0));
		break;
	case CinematicEditor::EventType::ScreenFade:
		ev = new FadeScreen(true);
		break;
	case CinematicEditor::EventType::MoveObject:
		ev = new MoveObject(V3F(0, 0, 0), V3F(0, 0, 0),myScene);
		break;
	case CinematicEditor::EventType::PlayAnimation:
		ev = new PlayAnimation(myScene);
		break;
	//case CinematicEditor::EventType::LookAt:
	//	ev = new CinematicLookAt(myScene);
	//	break;
	}
	if (ev)
	{
		myState->AddEvent(ev);
	}
}

void CinematicEditor::Test()
{
	FlattenPanning();
	VerifyTiming();
	Message mess;
	mess.myMessageType = MessageType::PushState;
	mess.myData = myState;
	PostMaster::GetInstance()->SendMessages(mess);
}

namespace MÅNSMETODERFÖRATTBAKAGODAPAJERTILLSINFARMORPÅENVARMSOMMAREFTERMIDDAGMUMS
{
	bool sortbysec(const std::pair<float, V3F>& a, const std::pair<float, V3F>& b)
	{
		return (a.first < b.first);
	}
}
void CinematicEditor::FlattenPanning()
{
	std::vector<std::pair<float, V3F>> cameraPositions;
	cameraPositions.push_back({ 0,myState->myCameraStartPos });
	for (auto& i : myState->myEvents)
	{
		CameraPan* pan = dynamic_cast<CameraPan*>(i);
		if (pan)
		{
			cameraPositions.push_back({ i->myStart + i->myDuration,pan->myPath.Evaluate(1.f)});
		}
	}

	std::sort(cameraPositions.begin(), cameraPositions.end(),&MÅNSMETODERFÖRATTBAKAGODAPAJERTILLSINFARMORPÅENVARMSOMMAREFTERMIDDAGMUMS::sortbysec);

	for (auto& i : myState->myEvents)
	{
		CameraPan* pan = dynamic_cast<CameraPan*>(i);
		if (pan)
		{
			size_t bucket = 0;
			float at = i->myStart;
			for (auto& pos : cameraPositions)
			{
				if (pos.first <= at)
				{
					bucket++;
				}
			}
			pan->myPath.SetStart(cameraPositions[bucket-1].second);
		}
	}
}

void CinematicEditor::VerifyTiming()
{
	float last = 0.f;
	for (auto& i : myState->myEvents)
	{
		last = MAX(i->myStart + i->myDuration, last);
	}
	myState->myTotalTime = last;
}
