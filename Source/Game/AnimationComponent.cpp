#include "pch.h"
#include "AnimationComponent.h"
#include "Mesh.h"
#include "Model.h"
#include "Animator.h"
#include "Entity.h"
#include "Random.h"
#include "Collision.h"
#include "AssetManager.h"


void AnimationComponent::Init(Entity* aEntity)
{
	LOGVERBOSE("AnimationComponent Inited");
	myEntity = aEntity;
	Mesh* mesh = aEntity->GetComponent<Mesh>();
	if (mesh)
	{
		AttachToMesh(mesh);
	}
	else
	{
		LOGWARNING("Animation component need to be added after a mesh component");
	}
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
			AnimationTrack track;
			if (!myAnimations[myNextState].empty())
			{
				track.myAnimation = myAnimations[myNextState].back();

				if (track.myAnimation.IsValid())
				{
					myCurrentLooping = myAnimator->AddAnimation(track);
				}
				else
				{
					SYSERROR("Starting animation unloaded", std::to_string(static_cast<int>(myNextState)));
				}
			}
			else
			{
				SYSERROR("Starting state does not have any animations loaded", std::to_string(static_cast<int>(myNextState)));
			}
			myHasAttackedToModel = true;
		}
		else
		{
			return;
		}
	}
	if (!myAnimator)
	{
		LOGWARNING("You need to call init on Animation components");
		return;
	}

	{
		PERFORMANCETAG("Update")
		myAnimator->Update(aDeltaTime);
	}
}

void AnimationComponent::Reset()
{
	LOGVERBOSE("AnimationComponent Reset")

	myCurrentState = AnimationComponent::States::Idle;
	myNextState = AnimationComponent::States::Idle;

	delete myAnimator;
	myAnimator = nullptr;

	myAnimations.clear();

	myModelImWaitingFor = nullptr;
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

void AnimationComponent::AttachToMesh(Mesh* aMeshcomponent)
{
	LOGVERBOSE("AnimationComponent Attached To mesh");
	ModelInstance* model = aMeshcomponent->GetModelInstance();
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
	Model::ModelData* modelData = aModel->GetModelAsset().GetAsModel()->GetModelData();
	myAnimator = new Animator();
	{
		PERFORMANCETAG("Parse");
		if (modelData->myAnimations.IsValid())
		{
			ParseAnimations(modelData->myAnimations, myAnimations);
		}
	}
	{
		PERFORMANCETAG("Init");
		myAnimator->Init(&aModel->GetModelAsset().GetAsModel()->myBoneData);
	}
	myModelImWaitingFor->AttachAnimator(myAnimator);
}

void AnimationComponent::SetState(States aNewState)
{
	if (!this)
	{
		return;
	}

	if (aNewState == myNextState)
	{
		return;
	}

	myNextState = aNewState;
	if (!myAnimator || myModelImWaitingFor)
	{
		return;
	}

	if (myCurrentLooping != Animator::TrackID())
	{
		myAnimator->FadeAnimation(myCurrentLooping, 0.2f);
	}

	if (myAnimations[aNewState].empty())
	{
		SYSERROR("new state does not have any animations loaded", std::to_string(static_cast<int>(aNewState)));
		return;
	}

	AnimationTrack track;
	track.myAnimation = myAnimations[aNewState].back();

	if (track.myAnimation.IsValid())
	{
		myCurrentLooping = myAnimator->AddAnimation(track);
	}
	else
	{
		SYSERROR("Switching to unloaded animation, skipping", std::to_string(static_cast<int>(aNewState)));
	}
}

void AnimationComponent::ParseAnimations(const AssetHandle& aAnimations, std::unordered_map<States, std::vector<AssetHandle>>& aOutAnimations)
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

	FiskJSON::Object& root = aAnimations.GetAsJSON();
	
	bool useStaticPaths = false;
	root["UseStaticPaths"].GetIf(useStaticPaths);

	for (auto& pool : root["Animations"])
	{
		if (stateMapping.count(pool.first) == 0)
		{
			SYSERROR("Unkown AnimationState", pool.first, aAnimations.GetJSONFilePath());
			continue;
		}
		if (pool.second->Is<FiskJSON::Array>())
		{
			for (auto& i : pool.second->Get<FiskJSON::Array>())
			{
				std::string path;
				if (!i->GetIf(path))
				{
					SYSERROR("Malformed animations file", "expected string", aAnimations.GetJSONFilePath());
					continue;
				}
				AssetHandle handle;
				if (useStaticPaths)
				{
					if (path.substr(0,2) == "./" || path.substr(0,2) == ".\\")
					{
						handle = AssetManager::GetInstance().GetAnimationRelative(aAnimations.GetJSONFilePath(), path.substr(2));
					}
					else
					{
						handle = AssetManager::GetInstance().GetAnimation(path);
					}
				}
				else
				{
					handle = AssetManager::GetInstance().GetAnimationRelative(aAnimations.GetJSONFilePath(), path);
				}

				aOutAnimations[stateMapping[pool.first]].push_back(handle);
			}
		}
		else
		{
			SYSERROR("Malformed animations file", "Animation pool expected array", aAnimations.GetJSONFilePath());
		}
	}

}

Animator* AnimationComponent::GetAnimator()
{
	return myAnimator;
}

bool AnimationComponent::HasAttachedToModel()
{
	return myHasAttackedToModel;
}