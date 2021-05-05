#include "pch.h"
#include "ParticleBurst.h"
#include "ParticleInstance.h"
#include "Scene.h"
#include "ParticleFactory.h"
#include "FiskJSON.h"
#if USEIMGUI
#include <imgui.h>
#endif
#ifdef _DEBUG
#include "DebugTools.h"
#endif

void ParticleBurst::Update(float aTime)
{
	if (IsActive(aTime))
	{
		if (myInstance)
		{
			if (myScene->Contains(myInstance))
			{
				myInstance->RefreshTimeout(1.f);
			}
			else
			{
				myInstance = nullptr;
			}
		}
		else
		{
			myInstance = myParticleFactory->InstantiateParticle(myType);
			if (myInstance)
			{
				myScene->AddInstance(myInstance);
				myInstance->SetBounds(V4F(myPosition - V3F(1, 1, 1) * myScale, 1), V4F(myPosition + V3F(1, 1, 1) * myScale, 1));
				myInstance->SetDirection(V4F(myDirection,0));
			}
		}
	}
	else
	{
		if (myInstance)
		{
			if (myScene->Contains(myInstance))
			{
				myInstance->RefreshTimeout(-1.f);
			}
			else
			{
				myInstance = nullptr;
			}
		}
	}
}


void ParticleBurst::AppendTo(FiskJSON::Object& aObject)
{
	FiskJSON::Object* me = new FiskJSON::Object();
	AppendCommon(*me);
	FiskJSON::Object* pos = new FiskJSON::Object();
	pos->MakeArray();
	for (auto& val : myPosition)
	{
		pos->PushValueChild(val);
	}
	me->AddChild("Position", pos);

	FiskJSON::Object* dir = new FiskJSON::Object();
	dir->MakeArray();
	for (auto& val : myDirection)
	{
		dir->PushValueChild(val);
	}
	me->AddChild("Direction", dir);

	me->AddValueChild("Area", myScale);
	me->AddValueChild("Path", myType);
	me->AddValueChild("CustomName", std::string(myName));

	aObject.PushChild(me);
}

void ParticleBurst::Edit()
{
#if USEIMGUI

	
	if (ImGui::InputText(".part", myNameBuffer, 128))
	{
		myType = std::string(myNameBuffer) + ".part";
	}
#ifdef _DEBUG
	ImGui::SameLine();
	if (ImGui::Button("Select"))
	{
		ImGui::OpenPopup("ParticleSelector");
	}

	if (ImGui::BeginPopup("ParticleSelector"))
	{
		ImGui::Text("Particles");
		ImGui::Separator();
		for (auto& particleFilePath : DebugTools::FileList->operator[](".part"))
		{
			std::string path = particleFilePath.substr(strlen("Data/Particles/"));
			if (ImGui::Selectable(path.c_str()))
			{
				WIPE(myNameBuffer);
				size_t dotpos = path.find('.');
				if (dotpos != std::string::npos)
				{
					memcpy(myNameBuffer, path.c_str(), dotpos * sizeof(char));
				}
				myType = path;
			}
		}
		ImGui::EndPopup();
	}
#endif // _DEBUG
	Tools::EditPosition("Position", myPosition);
	ImGui::DragFloat("Size", &myScale, 1, 0.0f, 1000.f);
	if (ImGui::Button("Test"))
	{
		myInstance = myParticleFactory->InstantiateParticle(myType);
		if (myInstance)
		{
			myScene->AddInstance(myInstance);
			myInstance->SetBounds(V4F(myPosition - V3F(1, 1, 1) * myScale, 1), V4F(myPosition + V3F(1, 1, 1) * myScale, 1));
			myInstance->SetDirection(V4F(myDirection, 0));
			myInstance->RefreshTimeout(GetDuration());
		}
	}

#endif // USEIMGUI
}