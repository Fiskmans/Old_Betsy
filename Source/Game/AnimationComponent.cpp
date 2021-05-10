#include "pch.h"
#include "AnimationComponent.h"
#include "Mesh.h"
#include "Model.h"
#include "Animator.h"
#include "Entity.h"
#include "Random.h"
#include "Collision.h"


void AnimationComponent::Init(Entity* aEntity)
{
	LOGVERBOSE("AnimationComponent Inited");
	myEntity = aEntity;
	myBlend = 0;
	Mesh* mesh = aEntity->GetComponent<Mesh>();
	if (mesh)
	{
		AttachToMesh(mesh);
	}
	else
	{
		LOGWARNING("Animation component need to be added after a mesh component");
	}
	//myEntity->AddActivity();
	myShouldAnimate = true;
	//SetState(States::Idle);
}

void AnimationComponent::Update(const float aDeltaTime)
{
	if (myModelImWaitingFor)
	{
		PERFORMANCETAG("Attach model")
			if (myModelImWaitingFor->GetModelAsset().GetAsModel()->ShouldRender())
			{
				AttachToModel(myModelImWaitingFor);
				myModelImWaitingFor = nullptr;
				myAnimator->SetState(IndexFromState(myNextState));
				myHasAttackedToModel = true;
			}
		return;
	}
	if (!myAnimator)
	{
		LOGWARNING("You need to call init on Animation components");
		return;
	}

	{
		PERFORMANCETAG("Update")
			myAnimator->Step(myShouldAnimate ? aDeltaTime : 0);
		//if (myShouldAnimate && myCurrentState != myNextState)
		//myBlend = 0;
		if (myShouldAnimate)
		{
			if (myBlend > 0.f)
			{
				myBlend -= aDeltaTime * myBlendSpeed;
				myAnimator->SetBlend(myBlend);
				if (myAnimator->DoneBlending())
				{
					myCurrentState = myNextState;
				}
			}
			else
			{
				myAnimator->SetBlend(0);
				if (myAnimator->DoneBlending())
				{
					myCurrentState = myNextState;
				}
			}
		}

		if (myAnimator->Looped())
		{
			switch (myCurrentState)
			{
			case States::Action:
				myEntity->SendEntityMessage(EntityMessage::ActionAnimationFinnished);
				break;

			case States::Interact:
				myEntity->SendEntityMessage(EntityMessage::InteractAnimationFinnished);
				break;
			
			case States::Unequip:
				myEntity->SendEntityMessage(EntityMessage::UnequipAnimationFinnished);
				break;

			case States::Equip:
				myEntity->SendEntityMessage(EntityMessage::EquipAnimationFinnished);
				break;

			case States::Cutting:
				myEntity->SendEntityMessage(EntityMessage::CuttingAnimationFinnished);
				break;
			default:
				break;
			}
		}
	}
}

void AnimationComponent::Reset()
{
	LOGVERBOSE("AnimationComponent Reset")

	myCurrentState = AnimationComponent::States::Idle;
	myNextState = AnimationComponent::States::Idle;

	delete myAnimator;
	myAnimator = nullptr;

	myStateMapping.clear();
	myModelImWaitingFor = nullptr;
	myBlend = 0.f;
	myShouldAnimate = true;
	myShouldUseEntityMessage = true;
	myHasAttackedToModel = false;
}

void AnimationComponent::OnAttach()
{
	LOGVERBOSE("AnimationComponent Attached")
}

void AnimationComponent::OnDetach()
{
	LOGVERBOSE("AnimationComponent Detached")
}

void AnimationComponent::OnKillMe()
{
	LOGVERBOSE("AnimationComponent Killed")
}

void AnimationComponent::RecieveEntityMessage(EntityMessage aMessage, void* someData)
{
	if (myShouldUseEntityMessage)
	{
		switch (aMessage)
		{
		case EntityMessage::StartDying:
			//SetState(States::Dying);
			break;
		case EntityMessage::StartFalling:
			//SetState(States::Falling);
			break;
		default:
			//NO-OP
			break;
		}
	}
}

void AnimationComponent::AlternativeInit(Entity* aEntity, int aModelIndex)
{
	LOGVERBOSE("AnimationComponent Inited");
	myEntity = aEntity;
	myBlend = 0;
	Mesh* mesh = aEntity->GetComponent<Mesh>();
	if (mesh)
	{
		AttachToMesh(mesh, aModelIndex);
	}
	else
	{
		LOGWARNING("Animation component need to be added after a mesh component");
	}
	myEntity->AddActivity();
	myShouldAnimate = true;
	SetState(States::Idle);
}

void AnimationComponent::AttachToMesh(Mesh* aMeshcomponent, int aModelIndex)
{
	LOGVERBOSE("AnimationComponent Attached To mesh");
	ModelInstance* model = aMeshcomponent->GetModelInstance(aModelIndex);
	if (model->GetModelAsset().GetAsModel()->ShouldRender())
	{
		myModelImWaitingFor = model;
		AttachToModel(model);
	}
	else
	{
		myModelImWaitingFor = model;
	}
}

void AnimationComponent::AttachToModel(ModelInstance* aModel)
{
	LOGVERBOSE("AnimationComponent attached To model");
	Model::CModelData* modelData = aModel->GetModelAsset().GetAsModel()->GetModelData();
	myAnimator = new Animator();
	std::vector<std::string> allAnimations;
	{
		PERFORMANCETAG("Parse");
		ParseAnimations(modelData->myAnimations, myStateMapping, allAnimations);
	}
	{
		PERFORMANCETAG("Init");
		myAnimator->Init(modelData->myFilePath, &aModel->GetModelAsset().GetAsModel()->myBoneData, allAnimations);
	}
	myModelImWaitingFor->AttachAnimator(myAnimator);
}

void AnimationComponent::SetState(States aNewState, int aRangeIndex, bool aForceState, bool aShouldUpateOld, bool aShouldBlend)
{
	if (!this)
	{
		return;
	}

	if (aNewState == myNextState && myCurrentRangeIndex == aRangeIndex)
	{
		return;
	}
	myCurrentRangeIndex = aRangeIndex;

	if (aForceState)
	{
		myShouldAnimate = true;
		myBlend = 0.001;
	}
	else
	{
		myBlend = 1;
	}

	if (!aShouldBlend)
	{
		myBlend = 0;
	}

	myNextState = aNewState;
	if (!myAnimator || myModelImWaitingFor)
	{
		return;
	}

	myAnimator->SetState(IndexFromState(myNextState, aRangeIndex), aShouldUpateOld);
	myAnimator->SetTime(0.f);
	//TODO: let old animation keep time , make smooth and silky :D
}

void AnimationComponent::ParseAnimations(const std::vector<std::string>& aAnimations, std::unordered_map<States, std::pair<size_t, size_t>>& aStatemapping, std::vector<std::string>& aFilteredAnimations)
{
	std::unordered_map<std::string, States> stateMapping;
	{
		stateMapping["Idle"] = States::Idle;
		stateMapping["Walk"] = States::Walking;
		stateMapping["Interact"] = States::Interact;
		stateMapping["Action"] = States::Action;
		stateMapping["Eating"] = States::Eating;
		stateMapping["Cutting"] = States::Cutting;
		stateMapping["Shake"] = States::Shake;
		stateMapping["Equip"] = States::Equip;
		stateMapping["Unequip"] = States::Unequip;
	}

	States state = States::Count;
	size_t start;
	size_t end;
	for (auto& i : aAnimations)
	{
		if (i.empty())
		{
			continue;
		}
		if ((i.length() > 0 && i[0] == '#') || (i.length() > 1 && i.substr(0, 2) == "//"))
		{
			continue;
		}
		auto it = stateMapping.find(i);
		if (it != stateMapping.end())
		{
			if (state != States::Count)
			{
				if (start <= aFilteredAnimations.size() - 1)
				{
					aStatemapping[state] = std::make_pair(start, aFilteredAnimations.size() - 1);
				}
			}
			state = it->second;
			start = aFilteredAnimations.size();
		}
		else
		{
			aFilteredAnimations.push_back("Data/Animations/" + i);
		}
	}
	if (state != States::Count)
	{
		if (start <= aFilteredAnimations.size() - 1)
		{
			aStatemapping[state] = std::make_pair(start, aFilteredAnimations.size() - 1);
		}
	}
}

void AnimationComponent::SetShouldUseEntityMessage(bool aVal)
{
	myShouldUseEntityMessage = aVal;
}

Animator* AnimationComponent::GetAnimator()
{
	return myAnimator;
}

bool AnimationComponent::HasAttachedToModel()
{
	return myHasAttackedToModel;
}

size_t AnimationComponent::IndexFromState(States aState, int aRangeIndex)
{
	if (myStateMapping.count(aState) == 0)
	{
		switch (aState)
		{
		/*case AnimationComponent::States::AttackMelee:
			ONETIMEWARNING("Missing mapping for attack2 defaulting to 1", "");
			return IndexFromState(States::AttackMelee);*/
			/*case AnimationComponent::States::Attack3:
				ONETIMEWARNING("Missing mapping for attack3 defaulting to 2","");
				return IndexFromState(States::Attack2);
			case AnimationComponent::States::Attack4:
				ONETIMEWARNING("Missing mapping for attack4 defaulting to 3","");
				return IndexFromState(States::Attack3);
			case AnimationComponent::States::Attack5:
				ONETIMEWARNING("Missing mapping for attack5 defaulting to 4","");
				return IndexFromState(States::Attack4);
			case AnimationComponent::States::Attack6:
				ONETIMEWARNING("Missing mapping for attack6 defaulting to 5","");
				return IndexFromState(States::Attack5);*/
		}

		LOGERROR("Model missing animation for state " + std::to_string(static_cast<int>(aState)));
		myStateMapping[aState] = std::make_pair(0ULL, 0ULL);
		return 0;
	}
	std::pair<size_t, size_t> range = myStateMapping[aState];
	if (range.second < range.first)
	{
		LOGWARNING("Animation range for " + std::to_string(static_cast<int>(aState)) + " is missing");
		return 0;
	}

	size_t result;
	//result = Tools::RandomRange(range.first, range.second);
	
	result = range.first + MIN(aRangeIndex, range.second - range.first);

	SYSVERBOSE("Swapping to animationstate[" + std::to_string(range.first) + "," + std::to_string(range.second) + "]: " + std::to_string(result));
	return result;
}
