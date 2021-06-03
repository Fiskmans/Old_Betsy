#pragma once
#include "Component.h"
#include "Observer.hpp"
#include "Publisher.hpp"
#include "CommonUtilities\Plane.hpp"

class Entity;
class Camera;
class SlabRay;
class Octree;
class GBPhysX;
class FollowCamera;

class AnimationComponent;

struct CuttingSheepEvent
{
	Entity* SheepEntity = nullptr;
	float timer = 0;
};

enum class InputWeaponState
{
	Idle,
	Equip,
	Unequip,
};

enum class Action
{
	Hoeing,
	Watering,
	Planting,
	Harvesting,
	GettingWater,
	PickingEggs,
	CuttingWool,
	LeavingHen,
	PickEggs
};

enum class EquipmentID
{
	None,
	Hoe,
	Seeds,
	WaterBucket,
	Basket,
	Scissor,
	Chicken
};

class PlayerController : public Component, public Observer, public Publisher
{
public:

	PlayerController();
	~PlayerController();

	virtual void Init(Entity* aEntity) override;
	void InternalInit(GBPhysX* aGBPhysX);
	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;

	void ResetControlerInput();

	void RecieveMessage(const Message& aMessage) override;
	//void SetCamera(Camera* aCamera, Entity* aCameraEntity);

	virtual void RecieveEntityMessage(EntityMessage aMessage, void* someData) override;

	void SetInputBlock(bool aInputBlocked)
	{
		myInputIsBlocked = aInputBlocked;
	}

	void SetFollowCameraPtr(FollowCamera* aFollowCam);
	void SetShouldBlockEverything(bool aBool);

	void SetSpawnPoint(V3F aSpawnPoint);

	void StopMoving();

protected:
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnKillMe() override;

private:
	bool Interact(Entity* aEntity);
	bool CheckInteractDistance(Entity* aEntity);

	void HandleInputStates();
	void HandleCharacterMovement();

	void SubscribeToMessages();
	void UnsubscribeToMessages();

	bool myCanMove = false;

	V3F Unproject(const V3F& aViewportPoint);
	SlabRay GetRayFromMousePos(V2F aMousePos);
	void CheckMouseRayVSOctree(SlabRay& r);
	V3F GetRayVsMapPoint(SlabRay& aRay);
	CommonUtilities::Plane<float> myMapPlane;

	Octree* myOctree;
	V2F myCurrentMousePos;
	SlabRay* myMouseRay;
	Entity* myCurrentMousedOverEntity;
	Entity* myLookAtInteractable;
	Entity* myClickedInteractable;
	GBPhysX* myGBPhysX;

	bool myInputIsBlocked = false;
	bool myIsDoingAction = false;

	V3F mySpawnPoint;

	V3F myInputMovement;
	V3F myMouseMovement;
	V3F myLastMouseMovement;
	V3F myCurrentMeshPosOffset;
	V3F myCurrentMeshRotOffset;

	std::vector<V3F> myWayPoints;

	CuttingSheepEvent myCuttingSheepEvent;

	//InputMovementState myInputMovementState;

	//GAMEMETRIC(float, mySensitivity, SENSITIVITY, 1.0f);
	//GAMEMETRIC(float, myCameraOffset, CAMERAHEIGHT, 50.f);

	void CheckForInteractable();

	/*Equipment myEquipedItem = Equipment::None;
	Equipment myItemToEquip = Equipment::None;*/

	Action myAction;

	FollowCamera* myFollowCameraPtr;

	MessageType myLastHoverMessage = MessageType::None;
	bool myShouldBlockEverything = true;
};