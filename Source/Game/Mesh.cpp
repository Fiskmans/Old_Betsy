#include "pch.h"
#include "Mesh.h"
#include "ModelLoader.h"
#include "Scene.h"
#include "Entity.h"
#include "Macros.h"
#include <algorithm>
#include "Macros.h"
#include <Model.h>


Mesh::Mesh() :
	myModelLoader(nullptr),
	myScene(nullptr),
	myIsFadingOut(false),
	myUpdateOverridden(false),
	myModelInstance(nullptr)
{
	myOffsetPosition = mySecondOffsetPosition = myOffsetRotation = V3F(0.f, 0.f, 0.f);
}

Mesh::~Mesh()
{
	Reset();
}

void Mesh::PreInit(ModelLoader* aModelLoader, Scene* aScene)
{
	myModelLoader = aModelLoader;
	myScene = aScene;
	myIsFadingOut = false;
	myTint = V4F(0, 0, 0, 1);
}

void Mesh::Init(Entity* aEntity)
{
	myEntity = aEntity;
	SubscribeToMyMessages();
	myScale = V3F(1, 1, 1);
	myModelInstance = nullptr;
}

void Mesh::SetUpModel(const std::string& aPath)
{
	myModelInstance = myModelLoader->InstantiateModel(aPath);
	SubscribeToMyMessages();
}

void Mesh::AddModelToScene(int aModelIndex)
{

	if (myModelInstance)
	{
		myScene->RemoveModel(myModelInstance);

		//UpdateInstancePosition();
		myScene->AddToScene(myModelInstance);
	}
	else
	{
		LOGWARNING("Mesh component added to scene without seting up the model")
	}
}

void Mesh::RemoveCurrentModelFromScene()
{
	myScene->RemoveModel(myModelInstance);
}

void Mesh::Update(const float aDeltaTime)
{
	float delta = -myFalloff * aDeltaTime / 2.0f;
	if (myIsFadingOut)
	{
		if (myTint.w < 0.f)
		{
			LOGVERBOSE("Mesh finished fading")
				myEntity->FinishActivity();
			myIsFadingOut = false;
		}
		myTint.w -= aDeltaTime * 0.1f;
	}
	UpdateInstancePosition();
}
void Mesh::UpdateInstancePosition()
{
	CommonUtilities::Vector3<float> position = myEntity->GetPosition() + myOffsetPosition + mySecondOffsetPosition;

	//turn model around so it faces away
	CommonUtilities::Matrix4x4<float> flipZ = CommonUtilities::Matrix4x4<float>::CreateRotationAroundY(PI);
	CommonUtilities::Matrix4x4<float> mat = flipZ * myEntity->GetRotation();
	mat = CommonUtilities::Matrix4x4<float>::CreateRotationAroundX(myPostRotation.x + myOffsetRotation.x) * mat;
	mat = CommonUtilities::Matrix4x4<float>::CreateRotationAroundY(myPostRotation.y + myOffsetRotation.y) * mat;
	mat = CommonUtilities::Matrix4x4<float>::CreateRotationAroundZ(myPostRotation.z + myOffsetRotation.z) * mat;

	auto instance = myModelInstance;
	if (instance != nullptr)
	{
		instance->SetPosition(CommonUtilities::Vector4<float>(position.x, position.y, position.z, 1.0f));
		instance->SetRotation(mat);
#ifdef _DEBUG
		if (instance->GetModel()->myIsMissNamed)
		{
			instance->SetTint(V4F(1, 0, 0, 1));
			DebugDrawer::GetInstance().SetColor(V4F(1, 0, 0, 1));
			DebugDrawer::GetInstance().DrawCross(position + V4F(0, 100, 0, 0), 30);
		}
		else
		{
#endif
			instance->SetTint(myTint);
#ifdef _DEBUG
		}
#endif // _DEBUG
	}
}

void Mesh::Reset()
{
	//TODO CHECK IF THIS IS SUPPOSED TO BE HERE
	UnSubscribeToMyMessages();

	myScene->RemoveModel(myModelInstance);
	SAFE_DELETE(myModelInstance);

	myIsFadingOut = false;
	myTint = V4F(0, 0, 0, 1);
	myUpdateOverridden = false;
	myEntity = nullptr;

	myPostRotation = V3F(0, 0, 0);
	myScale = V3F(1, 1, 1);
	myOffsetPosition = V3F(0, 0, 0);
	mySecondOffsetPosition = V3F(0, 0, 0);
	myOffsetRotation = V3F(0, 0, 0);

	LOGINFO("removed model from scene");
}

ModelInstance* Mesh::GetModelInstance(int aModelIndex)
{
	return myModelInstance;
}

int Mesh::GetNumberOfMeshes()
{
	return 1;
}

void Mesh::SubscribeToMyMessages()
{
	//SubscribeToMessage(MessageType::NewLevelLoaded);
}

void Mesh::UnSubscribeToMyMessages()
{
	//UnSubscribeToMessage(MessageType::NewLevelLoaded);
}

void Mesh::RecieveEntityMessage(EntityMessage aMessage, void* someData)
{
	if (aMessage == EntityMessage::DeathAnimationOver)
	{
		LOGVERBOSE("Mesh started fading");
		myEntity->AddActivity();
		myIsFadingOut = true;
	}

	//if (!myModelInstance->ShouldBeDrawnThroughWalls() && myEntity->GetEntityType() == EntityType::Enemy && (aMessage == EntityMessage::AttackAnimationOver || aMessage == EntityMessage::StartWalking)) //Attack over also means "spawn over" because reasons
	//{
		//myModelInstance->SetShouldBeDrawnThroughWalls(true);
	//}
}

void Mesh::OnAttach()
{
}

void Mesh::OnDetach()
{
}

void Mesh::OnKillMe()
{
}

void Mesh::RecieveMessage(const Message& aMessage)
{
	switch (aMessage.myMessageType)
	{
	case MessageType::NewLevelLoaded:
		//if (myScene && myModelInstance)
		//{
		//	//myScene->AddModel(myModelInstance);
		//}
		break;
	default:
		LOGWARNING("Unkown message type in mesh component")
			break;
	}
}

void Mesh::SetTint(V4F aTint)
{
	myTint = aTint;

	myModelInstance->SetTint(aTint);
}

void Mesh::SetPostRotation(V3F aPostRotation)
{
	myPostRotation = aPostRotation;
}

void Mesh::SetScale(V3F aScale, int aModelIndex)
{
	myModelInstance->SetScale(aScale);
}

void Mesh::SetOffsetPosition(V3F aOffset)
{
	myOffsetPosition = aOffset;
}

void Mesh::SetSecondOffsetPosition(V3F aOffset)
{
	mySecondOffsetPosition = aOffset;
}

void Mesh::SetOffsetRotation(V3F aOffset)
{
	myOffsetRotation = aOffset;
}

void Mesh::SetShouldBeDrawnThroughWalls(const bool aFlag)
{
	myModelInstance->SetShouldBeDrawnThroughWalls(aFlag);
}

void Mesh::SetFading(bool aIsFading)
{
	myIsFadingOut = false;
}

void Mesh::SetUsePlayerThroughWallShader(const bool aFlag)
{
	myModelInstance->SetUsePlayerThroughWallShader(aFlag);
}

void Mesh::SetModelLimit(int aLimit)
{
	//NO-OP
}


void Mesh::AttachToBone(ModelInstance* aParentEntity, size_t aBone, int aModelIndex)
{
	myModelInstance->AttachToBone(aParentEntity, aBone);
}

void Mesh::SetCastsShadows(bool aValue)
{
	myModelInstance->SetCastsShadows(aValue);
}
