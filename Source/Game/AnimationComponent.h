#pragma once
#include "Component.h"
#include <MetricValue.h>
#include "Macros.h"
#include <unordered_map>
class Mesh;
class Animator;
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
	virtual void RecieveEntityMessage(EntityMessage aMessage, void* someData) override;

	void AlternativeInit(Entity* aEntity, int aModelIndex);

	void SetState(States aNewState, int aRangeIndex = 0, bool aForceState = false, bool aShouldUpdateOld = true, bool aShouldBlend = true);

	static void ParseAnimations(const std::vector<std::string>& aAnimations, std::unordered_map<States, std::pair<size_t, size_t>>& aStatemapping, std::vector<std::string>& aFilteredAnimations);
	void SetShouldUseEntityMessage(bool aVal);

	States GetCurrentState()
	{
		return myCurrentState;
	}
	Animator* GetAnimator();

	bool HasAttachedToModel();
private:
	States myAttackState;
	size_t IndexFromState(States aState, int aRangeIndex = 0);
	void AttachToMesh(Mesh* aMeshcomponent, int aModelindex = 0);
	void AttachToModel(ModelInstance* aModel);

	GAMEMETRIC(float, myBlendSpeed, ANIMATIONGLOBALBLENDSPEED, 2.f);

	std::unordered_map<States, std::pair<size_t, size_t>> myStateMapping;
	Animator* myAnimator = nullptr;
	States myCurrentState = States::Idle;
	States myNextState = States::Idle;
	int myCurrentRangeIndex;
	ModelInstance* myModelImWaitingFor = nullptr;
	float myBlend = 0.f;
	bool myShouldAnimate = true;
	bool myShouldUseEntityMessage = true;
	bool myHasAttackedToModel = false;
};

