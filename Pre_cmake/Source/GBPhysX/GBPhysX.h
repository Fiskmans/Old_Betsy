#pragma once
#include "AnimationData.h"
#include "GamlaBettan\AssImp\scene.h"

namespace physx
{
	class PxRigidActor;
	class PxController;
	struct PxFilterData;
	class PxArticulation;
	class PxArticulationLink;
}
class GBPhysX;
class GBPhysXActor;
class StaticMeshCooker;

namespace GBPhysXFilters
{
	enum CollisionFilter : UINT32
	{
		None = 0,
		Player = 1 << 0,
		Enemy = 1 << 1,
		Bullet = 1 << 2,
		EnvironmentStatic = 1 << 3,
		EnvironmentMoveBlock = 1 << 4,
		EnvironmentDynamic = 1 << 5,
		EnemyHitBox = 1 << 6,
		NavMesh = 1 << 7,
		All = ~0U
	};
}

enum class GBPhysXGeometryType
{
	Cube,
	Sphere,
	Mesh,
	Count,
	None
};

struct HitResult
{
	GBPhysXActor* actor = nullptr;
	int faceIndex = 0;
	V3F position;
	V3F normal;
};

class GBPhysXActor
{
	friend GBPhysX;
public:
	GBPhysXActor();
	~GBPhysXActor();

	M44f GetTransformMatrix();
	V3F GetPosition();
	void RemoveFromScene();
	void Release();
	void SetIsKinematic(bool aIsKinematic);
	void SetKinematicTargetPos(V3F aPos);
	void ApplyForce(V3F aDirection, float aMagnitude);

	void SleepActor();

	void SetGBPhysXPtr(GBPhysX* aGBPhysXPtr);

	void SetShapeName(const std::string& aName);

	void SetRigidActor(physx::PxRigidActor* aRigidActor);
	physx::PxRigidActor* GetRigidActor();

	void SetIsHitBox(bool aIsHitBox);
	bool GetIsHitBox();

	bool GetHasBeenReleased();

	std::string GetName();
protected:
	GBPhysX* myGBPhysX = nullptr;
	physx::PxRigidActor* myRigidActor = nullptr;
	bool myActorHasBeenReleased = false;
private:
	bool myIsKinematic = false;
	bool myIsHitBox = false;
};

class GBPhysXHitBox : public GBPhysXActor
{
public:
	void SetHitBox(HitBox aHitBox);
	HitBox& GetHitBox();
private:
	HitBox myHitBox;
};

class GBPhysXCharacter : public GBPhysXActor
{
	friend GBPhysX;
public:
	GBPhysXCharacter();
	~GBPhysXCharacter();

	void Init(bool aIsPlayer);
	void Move(V3F aDirection, float aDeltaTime);
	void SetIsGrounded(bool aIsGrounded);
	bool GetIsGrounded() const;
	void Jump();
	void Crouch();
	void Stand();
	bool GetIsCrouching() const;
	void Teleport(V3F aPosition);
	void CreateHitBoxes();
	void UpdateHitBoxes();
	void Kill();
	void ReleaseHitBoxes();
	void SetArticulation(physx::PxArticulation* aArticulation);
	void SetLinkToLookAt(std::array<physx::PxArticulationLink*, 64>& aLinkToLookAtArray);
	
	void SetDeathMomentBoneTransformations(std::array<CommonUtilities::Matrix4x4<float>, 64> aMatrixes);
	void SetDeathMomentCharTransform(M44f& aTransform);
private:
	physx::PxController* myController = nullptr;
	physx::PxFilterData* myFilterData = nullptr;
	bool myIsCrouching;
	bool myIsGrounded;
	float myCurrentFallVelocity = 0.0f;
	std::vector<GBPhysXHitBox*> myHitBoxes;
	physx::PxArticulation* myArticulation = nullptr;
	std::array<CommonUtilities::Matrix4x4<float>, 64> myDeathMomentBoneTransformations;
	M44f myDeathMomentCharTransform;
	std::array<physx::PxArticulationLink*, 64> myLinkToLookAt;
};



class GBPhysX
{
public:
	~GBPhysX();

	GBPhysXActor* GBCreateDynamicSphere(M44f aMatrixTransform, int aRadius, float aDensity = 10.0f);
	GBPhysXActor* GBCreateDynamicBox(V3F aPosition, V3F aSize, V3F aForce, float aDensity);
	GBPhysXActor* GBCreateStaticCube(V3F aPosition, float aHalfSize);
	GBPhysXActor* GBCreatePlayerBlockBox(V3F aPosition, V3F aSize, V3F aRotation);
	GBPhysXActor* GBCreateKinematicBox(V3F aPosition, V3F aSize, V3F aForce, float aDensity);
	GBPhysXActor* GBCreateTriangleMesh(const char* aVertexData, size_t aVertexStride, size_t aVertexCount);

	std::vector<GBPhysXActor*> GBCreateChain(V3F aPosition, V3F aSize, int aLength, float aSeparation);

	GBPhysXCharacter* GBCreateCapsuleController(V3F aPosition, V3F aRotation, float aHeight, float aRadius, bool aIsPlayer);

	HitResult RayCast(FRay aRay, float aRange = 10_m);

	void GBCreateNavMesh(int aNumberOfVerts, std::vector<V3F> someVerts, int aNumberOfTriangles, std::vector<int> someIndices);
	void GBInitPhysics(bool aInteractive);
	void GBStepPhysics(float aDeltaTime);
	void GBCleanUpPhysics();
	bool GetGBPhysXActive();

	void GBSetKinematicActorTargetPos(physx::PxRigidActor* aActor, V3F aPosition, M44f aRotation);
	void GBApplyForceToActor(physx::PxRigidActor* aActor, V3F aForce);
	void GBResetScene();
	void RemoveActor(physx::PxRigidActor* aRigidActor);
	void ReleaseActor(physx::PxRigidActor* aRigidActor);

	GBPhysXActor* CreateStaticTriangleMeshObject(std::string& aFilePath, M44f aTransform);
	bool CookStaticTriangleMesh(std::string& aFilePath, const aiScene* aAiScene, aiNode* aAiNode);

	void SetZombieHitBoxData(std::vector<HitBox>& aHitBoxes);
	std::vector<HitBox>& GetZombieHitBoxes();

	void SaveNodeParents(std::unordered_map<int, int> nodeParents);
	std::unordered_map<int, int> myRagdollNodeParents;

private:

	void SetGBPhysXActive(bool aActive);
	bool myIsActive = false;
	float myDeltaTime = 0;
	StaticMeshCooker* myStaticMeshCooker = nullptr;
	std::vector<HitBox> myZombieHitBoxes;
};