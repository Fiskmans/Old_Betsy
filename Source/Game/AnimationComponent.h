#pragma once
#include "Component.h"
#include <MetricValue.h>
#include "Macros.h"
#include <unordered_map>
#include "Animator.h"

class Mesh;
class ModelInstance;

class AnimationComponent : public Component
{
public:
	enum class States
	{
		Idle,
		Walking,
		Interact,
		Action,
		Eating,
		Cutting,
		Shake,
		Equip,
		Unequip,
		Cinematic,
		Count
	};

	// Inherited via Component
	virtual void Init(Entity* aEntity) override;
	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnKillMe() override;

	void SetState(States aNewState);

	static void ParseAnimations(const AssetHandle& aAnimations, std::unordered_map<States, std::vector<AssetHandle>>& aOutAnimations);

	States GetCurrentState()
	{
		return myCurrentState;
	}
	Animator* GetAnimator();

	bool HasAttachedToModel();
private:
	States myAttackState;
	void AttachToMesh(Mesh* aMeshcomponent);
	void AttachToModel(ModelInstance* aModel);

	std::unordered_map<States, std::vector<AssetHandle>> myAnimations;
	Animator* myAnimator = nullptr;
	States myCurrentState = States::Idle;
	States myNextState = States::Idle;
	ModelInstance* myModelImWaitingFor = nullptr;
	bool myHasAttackedToModel = false;

	Animator::TrackID myCurrentLooping;
};

