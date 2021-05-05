#include <pch.h>

#pragma warning(push, 1)
#pragma warning(disable : 26495)
#include "GBPhysX.h"
#include "SnippetUtils.h"
#include "PxPhysicsAPI.h"
#include "PxRigidActor.h"
#include "Matrix4x4.hpp"
#include <array>
#include "GBPhysXLibraryDependencies.h"
#include "GBPhysXQueryFilterCallback.h"
#include "GBPhysXSimulationEventCallback.h"
#include "GBPhysXSimulationFilterCallback.h"
#include "GBPhysXControllerHitReportCallback.h"
#include "StaticMeshCooker.h"
#pragma warning(pop)
#include "Entity.h"
#include "AnimationComponent.h"
#include "Animator.h"
#include "mesh.h"

using namespace physx;

#define PlayerGravityModifier 3.5f
#define PlayerHeight 100.0f
#define PlayerRadius 50.0f

static PxDefaultAllocator		gAllocator;
static PxDefaultErrorCallback	gErrorCallback;

PxFoundation* gFoundation = NULL;
PxPhysics* gPhysics = NULL;

PxDefaultCpuDispatcher* gDispatcher = NULL;
PxScene* gScene = NULL;
PxCooking* gCooking = NULL;
PxMaterial* gMaterial = NULL;

PxPvd* gPvd = NULL;
PxReal stackZ = 10.0f;

PxControllerManager* manager = NULL;

PxSimulationEventCallback* simulationEventCallback = NULL;
PxSimulationFilterCallback* simulationFilterCallback = NULL;
PxQueryFilterCallback* queryFilterCallback = NULL;
GBPhysXBulletQueryFilterCallback* bulletQueryFilterCallback = NULL;
GBPhysXInteractQueryFilterCallback* interactQueryFilterCallback = NULL;
GBPhysXCCTQueryFilterCallback* cctQueryFilterCallback = NULL;


// Setup common cooking params
void setupCommonCookingParams(PxCookingParams& params, bool skipMeshCleanup, bool skipEdgeData)
{
	// we suppress the triangle mesh remap table computation to gain some speed, as we will not need it 
	// in this snippet
	params.suppressTriangleMeshRemapTable = true;

	// If DISABLE_CLEAN_MESH is set, the mesh is not cleaned during the cooking. The input mesh must be valid. 
	// The following conditions are true for a valid triangle mesh :
	//  1. There are no duplicate vertices(within specified vertexWeldTolerance.See PxCookingParams::meshWeldTolerance)
	//  2. There are no large triangles(within specified PxTolerancesScale.)
	// It is recommended to run a separate validation check in debug/checked builds, see below.

	if (!skipMeshCleanup)
		params.meshPreprocessParams &= ~static_cast<PxMeshPreprocessingFlags>(PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH);
	else
		params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;

	// If DISABLE_ACTIVE_EDGES_PREDOCOMPUTE is set, the cooking does not compute the active (convex) edges, and instead 
	// marks all edges as active. This makes cooking faster but can slow down contact generation. This flag may change 
	// the collision behavior, as all edges of the triangle mesh will now be considered active.
	if (!skipEdgeData)
		params.meshPreprocessParams &= ~static_cast<PxMeshPreprocessingFlags>(PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE);
	else
		params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
}

// Creates a triangle mesh using BVH34 midphase with different settings.
PxRigidStatic* createBV34TriangleMesh(PxU32 numVertices, const PxVec3* vertices, PxU32 numTriangles, const PxU32* indices,
	bool skipMeshCleanup, bool skipEdgeData, bool inserted, const PxU32 numTrisPerLeaf)
{
	//PxU64 startTime = SnippetUtils::getCurrentTimeCounterValue();

	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = numVertices;
	meshDesc.points.data = vertices;
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.triangles.count = numTriangles;
	meshDesc.triangles.data = indices;
	meshDesc.triangles.stride = 3 * sizeof(PxU32);

	PxCookingParams params = gCooking->getParams();

	// Create BVH34 midphase
	params.midphaseDesc = PxMeshMidPhase::eBVH34;

	// setup common cooking params
	setupCommonCookingParams(params, skipMeshCleanup, skipEdgeData);

	// Cooking mesh with less triangles per leaf produces larger meshes with better runtime performance
	// and worse cooking performance. Cooking time is better when more triangles per leaf are used.
	params.midphaseDesc.mBVH34Desc.numPrimsPerLeaf = numTrisPerLeaf;

	gCooking->setParams(params);

#if defined(PX_CHECKED) || defined(PX_DEBUG)
	// If DISABLE_CLEAN_MESH is set, the mesh is not cleaned during the cooking. 
	// We should check the validity of provided triangles in debug/checked builds though.
	if (skipMeshCleanup)
	{
		PX_ASSERT(gCooking->validateTriangleMesh(meshDesc));
	}
#endif // DEBUG


	PxTriangleMesh* triMesh = NULL;
	PxU32 meshSize = 0;

	// The cooked mesh may either be saved to a stream for later loading, or inserted directly into PxPhysics.
	if (inserted)
	{
		triMesh = gCooking->createTriangleMesh(meshDesc, gPhysics->getPhysicsInsertionCallback());
	}
	else
	{
		PxDefaultMemoryOutputStream outBuffer;
		if (gCooking->cookTriangleMesh(meshDesc, outBuffer))
		{

		}
		else
		{
			SYSERROR("cooking of a mesh failed", "cooking failed");
			return nullptr;
		}

		PxDefaultMemoryInputData stream(outBuffer.getData(), outBuffer.getSize());
		triMesh = gPhysics->createTriangleMesh(stream);

		meshSize = outBuffer.getSize();
	}

	float identity[16] = { 1.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,0.0f,1.0f };
	PxRigidStatic* body = (gPhysics->createRigidStatic(PxTransform(PxMat44(identity))));
	PxShape* shape = gPhysics->createShape(PxTriangleMeshGeometry(triMesh), *gMaterial, true);
	body->attachShape(*shape);

	gScene->addActor(*body);

	triMesh->release();
	return body;
}

PxRigidStatic* createStaticNavMesh(PxU32 aNumVertices, PxVec3* someVertices, PxU32 aNumTriangles, PxU32* someIndices)
{
	PxRigidStatic* rigidStatic = createBV34TriangleMesh(aNumVertices, someVertices, aNumTriangles, someIndices, true, false, false, 15);

	physx::PxShape* shape;
	rigidStatic->getShapes(&shape, 1);
	rigidStatic->setName("ground");

	physx::PxFilterData filterData;
	filterData.word0 = GBPhysXFilters::CollisionFilter::NavMesh;
	filterData.word1 = GBPhysXFilters::CollisionFilter::Player | GBPhysXFilters::CollisionFilter::Enemy | GBPhysXFilters::CollisionFilter::EnvironmentDynamic;
	filterData.word3 = 1;

	shape->setSimulationFilterData(filterData);
	shape->setQueryFilterData(filterData);

	return rigidStatic;
}

PxRigidDynamic* createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity = PxVec3(0), const PxReal& aDensity = 10.0f)
{
	PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, t, geometry, *gMaterial, aDensity);
	if (dynamic)
	{
		dynamic->setAngularDamping(0.5f);
		dynamic->setLinearVelocity(velocity);
		gScene->addActor(*dynamic);
		return dynamic;
	}
	return nullptr;
}

PxRigidStatic* createStaticCube(const PxTransform& t, PxReal halfExtent)
{
	PxShape* shape = gPhysics->createShape(PxBoxGeometry(halfExtent, halfExtent, halfExtent), *gMaterial, true);
	PxRigidStatic* body = gPhysics->createRigidStatic(t);
	body->attachShape(*shape);
	gScene->addActor(*body);

	shape->release();
	return body;
}

PxRigidStatic* createStaticBox(const PxTransform& t, PxVec3 aSize)
{
	PxShape* shape = gPhysics->createShape(PxBoxGeometry(aSize), *gMaterial, true);
	PxRigidStatic* body = gPhysics->createRigidStatic(t);
	body->attachShape(*shape);
	gScene->addActor(*body);

	shape->release();
	return body;
}



// spherical joint limited to an angle of at most pi/4 radians (45 degrees)
PxJoint* createLimitedSpherical(PxRigidActor* a0, const PxTransform& t0, PxRigidActor* a1, const PxTransform& t1)
{
	PxSphericalJoint* j = PxSphericalJointCreate(*gPhysics, a0, t0, a1, t1);
	j->setLimitCone(PxJointLimitCone(PxPi / 4, PxPi / 4, 0.05f));
	j->setSphericalJointFlag(PxSphericalJointFlag::eLIMIT_ENABLED, true);
	return j;
}

// revolute joint limited to an angle of at most pi/4 radians (45 degrees)

// fixed, breakable joint
PxJoint* createBreakableFixed(PxRigidActor* a0, const PxTransform& t0, PxRigidActor* a1, const PxTransform& t1)
{
	PxFixedJoint* j = PxFixedJointCreate(*gPhysics, a0, t0, a1, t1);
	j->setBreakForce(1000, 100000);
	j->setConstraintFlag(PxConstraintFlag::eDRIVE_LIMITS_ARE_FORCES, true);
	j->setConstraintFlag(PxConstraintFlag::eDISABLE_PREPROCESSING, false);
	return j;
}

// D6 joint with a spring maintaining its position
PxJoint* createDampedD6(PxRigidActor* a0, const PxTransform& t0, PxRigidActor* a1, const PxTransform& t1)
{
	PxD6Joint* j = PxD6JointCreate(*gPhysics, a0, t0, a1, t1);
	j->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);
	j->setMotion(PxD6Axis::eSWING2, PxD6Motion::eFREE);
	j->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);
	j->setDrive(PxD6Drive::eSLERP, PxD6JointDrive(0, 1000, FLT_MAX, true));
	return j;
}

typedef PxJoint* (*JointCreateFunction)(PxRigidActor* a0, const PxTransform& t0, PxRigidActor* a1, const PxTransform& t1);

// create a chain rooted at the origin and extending along the x-axis, all transformed by the argument t.

std::vector<PxRigidDynamic*> createChain(const PxTransform& t, PxU32 length, const PxGeometry& g, PxReal separation, JointCreateFunction createJoint)
{
	std::vector<PxRigidDynamic*> vector;
	PxVec3 offset(separation / 2, 0, 0);
	PxTransform localTm(offset);
	PxRigidDynamic* prev = NULL;

	for (PxU32 i = 0; i < length; i++)
	{
		PxRigidDynamic* current = PxCreateDynamic(*gPhysics, t * localTm, g, *gMaterial, 10.0f);
		(*createJoint)(prev, prev ? PxTransform(offset) : t, current, PxTransform(-offset));
		gScene->addActor(*current);
		prev = current;
		localTm.p.x += separation;
		vector.push_back(current);
	}
	return vector;
}

physx::PxFilterFlags CollisionFilterShader(
	physx::PxFilterObjectAttributes aAttributes0, physx::PxFilterData aFilterData0,
	physx::PxFilterObjectAttributes aAttributes1, physx::PxFilterData aFilterData1,
	physx::PxPairFlags& aPairFlags, const void* aConstantBlock, PxU32 aConstantBlockSize)
{
	PX_UNUSED(aConstantBlock);
	PX_UNUSED(aConstantBlockSize);
	if ((aFilterData0.word0 & aFilterData1.word1) && (aFilterData1.word0 & aFilterData0.word1))
	{
		if (PxFilterObjectIsTrigger(aAttributes0) || PxFilterObjectIsTrigger(aAttributes1))
		{
			aPairFlags = PxPairFlag::eTRIGGER_DEFAULT;
			return PxFilterFlag::eDEFAULT;
		}

		if (PxFilterObjectIsKinematic(aAttributes0) && PxFilterObjectIsKinematic(aAttributes1))
		{
			aPairFlags = PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_LOST | PxPairFlag::eNOTIFY_TOUCH_PERSISTS | PxPairFlag::eNOTIFY_CONTACT_POINTS;
			return PxFilterFlag::eSUPPRESS;
		}

		aPairFlags = PxPairFlag::eSOLVE_CONTACT | PxPairFlag::eDETECT_DISCRETE_CONTACT | PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_LOST | PxPairFlag::eNOTIFY_TOUCH_CCD | PxPairFlag::eNOTIFY_CONTACT_POINTS | PxPairFlag::eDETECT_CCD_CONTACT;
		return PxFilterFlag::eDEFAULT;
	}
	return PxFilterFlag::eSUPPRESS;
}

void initPhysics(bool interactive)
{
	if (!gFoundation)
	{
		gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
	}

	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(LOCAL_HOST, 5425, 10);
	gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);
	gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, PxCookingParams(PxTolerancesScale()));
	PxInitExtensions(*gPhysics, gPvd);

	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	simulationEventCallback = new GBPhysXSimulationEventCallback();
	simulationFilterCallback = new GBPhysXSimulationFilterCallback();
	queryFilterCallback = new GBPhysXQueryFilterCallback();
	bulletQueryFilterCallback = new GBPhysXBulletQueryFilterCallback();
	interactQueryFilterCallback = new GBPhysXInteractQueryFilterCallback();
	cctQueryFilterCallback = new GBPhysXCCTQueryFilterCallback();

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -981.0f, 0.0f);
	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;

	sceneDesc.simulationEventCallback = simulationEventCallback;
	sceneDesc.filterCallback = simulationFilterCallback;
	sceneDesc.filterShader = CollisionFilterShader;

	gScene = gPhysics->createScene(sceneDesc);

	PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	manager = PxCreateControllerManager(*gScene);
}

void setKinematicActorTargetPos(PxRigidActor* aActor, PxTransform& aTarget)
{
	if (aActor != nullptr)
	{
		((PxRigidDynamic*)aActor)->setKinematicTarget(aTarget);
	}
}

void addForceToDynamicActor(PxRigidActor* aActor, PxVec3& aForce)
{
	//((PxRigidDynamic*)aActor)->addForce(aForce, PxForceMode::eFORCE, true);
	((PxRigidDynamic*)aActor)->setLinearVelocity(aForce, true);
}

void stepPhysics(float aDeltaTime)
{
	{
		PERFORMANCETAG("Simulate");
		gScene->simulate(aDeltaTime);
	}
	{
		PERFORMANCETAG("Fetch");
		if (gScene->fetchResults(true))
		{
			//simulationEventCallback->HandleCollisions();
		}
	}
}

void cleanupPhysics(bool /*interactive*/)
{
	SAFE_DELETE(simulationEventCallback);
	SAFE_DELETE(queryFilterCallback);
	SAFE_DELETE(bulletQueryFilterCallback);

	PX_RELEASE(manager);
	PX_RELEASE(gMaterial);
	PX_RELEASE(gScene);
	PX_RELEASE(gDispatcher);
	PX_RELEASE(gPhysics);
	if (gPvd)
	{
		PxPvdTransport* transport = gPvd->getTransport();
		gPvd->release();
		gPvd = NULL;
		PX_RELEASE(transport);
	}
	PX_RELEASE(gCooking);


	PxCloseExtensions();
}


physx::PxTransform MatToTransform(const M44F& aMatrix)
{
	float transform[16] = { aMatrix(1,1),aMatrix(1,2),aMatrix(1,3),aMatrix(1,4),aMatrix(2,1),aMatrix(2,2),aMatrix(2,3),aMatrix(2,4),aMatrix(3,1),aMatrix(3,2),aMatrix(3,3),aMatrix(3,4), aMatrix(4,1), aMatrix(4,2),aMatrix(4,3),1.0f };
	PxMat44 matrix = PxMat44(transform);
	return PxTransform(matrix);
}

M44F TransformToMat(PxTransform& aTransform)
{
	PxMat44 mat = PxMat44(aTransform);
	M44F returnValue = M44F(mat.column0.x, mat.column1.x, mat.column2.x, mat.column3.x, mat.column0.y, mat.column1.y, mat.column2.y, mat.column3.y, mat.column0.z, mat.column1.z, mat.column2.z, mat.column3.z, mat.column0.w, mat.column1.w, mat.column2.w, mat.column3.w);
	return returnValue;
	return M44F::Transpose(returnValue);
}

PxCapsuleGeometry GetHitboxGeometry(HitBoxType aType)
{
	PxCapsuleGeometry geom;
	switch (aType)
	{
	case HitBoxType::Head:
		geom = PxCapsuleGeometry(10.0f, 2.0f);
		break;
	case HitBoxType::Neck:
		geom = PxCapsuleGeometry(4.0f, 2.0f);
		break;
	case HitBoxType::Hip:
		geom = PxCapsuleGeometry(2.0f, 1.0f);
		break;
	case HitBoxType::LowerTorso:
		geom = PxCapsuleGeometry(12.0f, 2.0f);
		break;
	case HitBoxType::LowerMidTorso:
		geom = PxCapsuleGeometry(12.0f, 2.0f);
		break;
	case HitBoxType::UpperMidTorso:
		geom = PxCapsuleGeometry(8.0f, 2.0f);
		break;
	case HitBoxType::UpperTorso:
		geom = PxCapsuleGeometry(2.0f, 1.0f);
		break;
	case HitBoxType::Torso:
		geom = PxCapsuleGeometry(8.0f, 4.0f);
		break;
	case HitBoxType::Shoulder:
		geom = PxCapsuleGeometry(4.0f, 4.0f);
		break;
	case HitBoxType::Clavicle:
		geom = PxCapsuleGeometry(2.0f, 1.0f);
		break;
	case HitBoxType::UpperArm:
		geom = PxCapsuleGeometry(7.0f, 10.0f);
		break;
	case HitBoxType::LowerArm:
		geom = PxCapsuleGeometry(5.0f, 8.0f);
		break;
	case HitBoxType::Hand:
		geom = PxCapsuleGeometry(3.0f, 3.50f);
		break;
	case HitBoxType::Thigh:
		geom = PxCapsuleGeometry(8.0f, 14.0f);
		break;
	case HitBoxType::LowerLeg:
		geom = PxCapsuleGeometry(6.0f, 15.0f);
		break;
	case HitBoxType::Toe:
		geom = PxCapsuleGeometry(4.0f, 4.0f);
		break;
	case HitBoxType::Foot:
		geom = PxCapsuleGeometry(4.0f, 4.0f);
		break;
	case HitBoxType::Count:
		geom = PxCapsuleGeometry(10.00f, 4.50f);
		break;
	case HitBoxType::None:
		geom = PxCapsuleGeometry(10.00f, 4.50f);
		break;
	default:
		geom = PxCapsuleGeometry(10.0f, 4.50f);
		break;
	}
	return geom;
}

PxArticulation* CreateArticulation(PxVec3 aWorldPos, GBPhysXCharacter* aCharacter, std::vector<GBPhysXHitBox*> aHitboxes, std::unordered_map<int, int> aNodeParents, V3F aDirection)
{
	// ANIMATION DATA
	std::array<CommonUtilities::Matrix4x4<float>, 64> boneTransforms;
	aCharacter->GetEntity()->GetComponent<AnimationComponent>()->GetAnimator()->BoneTransform(boneTransforms);
	aCharacter->SetDeathMomentBoneTransformations(boneTransforms);

	//CHARACTER DATA AT TIME OF CREATION FOR TRANSFORMING CHAR TO RIGHT PLACE
	M44F characterMatrix = aCharacter->GetTransformMatrix();
	characterMatrix[13] -= PlayerHeight;
	aCharacter->SetDeathMomentCharTransform(characterMatrix);

	//FUTURE NODEPARENTS FOR RAGDOLLING
	std::unordered_map<int, int> nodeparents = aNodeParents;

	// START ARTICULATION CREATION	---------------------------------------------------------------
	PxArticulation* articulation = gPhysics->createArticulation();

	//CREATE ROOT FROM HIP NODE
	auto rootTransform = boneTransforms[1];
	auto rootBoneOffset = aHitboxes[0]->GetHitBox().parentBoneData.BoneOffset;

	M44F rootoffsetInversed = rootBoneOffset;
	rootoffsetInversed = M44F::GetRealInverse(rootoffsetInversed);
	M44F rootMatrix = rootTransform * rootoffsetInversed;

	M44F rootcuMatrix(rootMatrix(1, 1), rootMatrix(1, 2), rootMatrix(1, 3), rootMatrix(1, 4),
		rootMatrix(2, 1), rootMatrix(2, 2), rootMatrix(2, 3), rootMatrix(2, 4),
		rootMatrix(3, 1), rootMatrix(3, 2), rootMatrix(3, 3), rootMatrix(3, 4),
		rootMatrix(4, 1), rootMatrix(4, 2), rootMatrix(4, 3), rootMatrix(4, 4));

	rootcuMatrix = M44F::Transpose(rootcuMatrix);
	M44F final = rootcuMatrix;

	PxTransform linkPose = MatToTransform(final);

	PxCapsuleGeometry* linkGeometry = new PxCapsuleGeometry(PxReal(2.0f), PxReal(4.0f));
	PxSphereGeometry* sphereLinkGeometry = new PxSphereGeometry(PxReal(4.0f));

	//HIP ROOT LINK
	PxArticulationLink* link = articulation->createLink(NULL, linkPose);

	PxShape* shape = PxRigidActorExt::createExclusiveShape(*link, *sphereLinkGeometry, *gMaterial);
	physx::PxFilterData filterData;
	filterData.word0 = GBPhysXFilters::CollisionFilter::Enemy;
	filterData.word1 = GBPhysXFilters::CollisionFilter::EnvironmentStatic | GBPhysXFilters::CollisionFilter::EnvironmentDynamic | GBPhysXFilters::CollisionFilter::Player;
	filterData.word3 = 1;
	shape->setSimulationFilterData(filterData);
	shape->setQueryFilterData(filterData);
	PxRigidBodyExt::updateMassAndInertia(*link, 0.01f);
	link->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);

	std::array<PxArticulationLink*, 64> createdLinks = std::array<PxArticulationLink*, 64>{nullptr};
	createdLinks[2] = link;
	
	// DEBUG DATA
#ifdef _DEBUG
	//int targetNodeIndex, link*

	std::unordered_map<int, std::string> createdLinksName;
	createdLinksName[2] = aHitboxes[0]->GetHitBox().myName;

	std::unordered_map<int, PxSphereGeometry> createdLinksGeometry;
	createdLinksGeometry[2] = *sphereLinkGeometry;

	std::unordered_map<int, V3F> createdLinksPositions;
	createdLinksPositions[2] = V3F(linkPose.p.x, linkPose.p.y, linkPose.p.z);
#endif // _DEBUG

	std::vector<GBPhysXHitBox> toBeSortedVector;
	for (int i = 0; i < aHitboxes.size(); i++)
	{
		if (i > 0)
		{
			toBeSortedVector.push_back(*aHitboxes[i]);
		}
	}

	for (auto& sortedHitBox : toBeSortedVector)
	{
		auto curr = sortedHitBox.GetHitBox();
		HitBoxType type = curr.myHitBoxType;
		int targetID = curr.myTargetNodeIndex;
		int originID = curr.myOriginNodeIndex;

		PxArticulationLink* parent;
		M44F transform = boneTransforms[curr.myOriginNodeIndex];
		auto BoneOffset = curr.parentBoneData.BoneOffset;

		M44F offsetInversed = BoneOffset;
		offsetInversed = M44F::GetRealInverse(offsetInversed);
		M44F matrix = transform * offsetInversed;

		M44F cuMatrix(matrix(1, 1), matrix(1, 2), matrix(1, 3), matrix(1, 4),
			matrix(2, 1), matrix(2, 2), matrix(2, 3), matrix(2, 4),
			matrix(3, 1), matrix(3, 2), matrix(3, 3), matrix(3, 4),
			matrix(4, 1), matrix(4, 2), matrix(4, 3), matrix(4, 4));

		cuMatrix = M44F::Transpose(cuMatrix);
		M44F final = cuMatrix;


		auto childBoneOffset = sortedHitBox.GetHitBox().childBoneData.BoneOffset;
		auto childTrans = boneTransforms[sortedHitBox.GetHitBox().myTargetNodeIndex];

		M44F childOffsetInversed = childBoneOffset;
		childOffsetInversed = M44F::GetRealInverse(childOffsetInversed);
		M44F childMatrix = childTrans * childOffsetInversed;

		M44F childCuMatrix(childMatrix(1, 1), childMatrix(1, 2), childMatrix(1, 3), childMatrix(1, 4),
			childMatrix(2, 1), childMatrix(2, 2), childMatrix(2, 3), childMatrix(2, 4),
			childMatrix(3, 1), childMatrix(3, 2), childMatrix(3, 3), childMatrix(3, 4),
			childMatrix(4, 1), childMatrix(4, 2), childMatrix(4, 3), childMatrix(4, 4));

		childCuMatrix = M44F::Transpose(childCuMatrix);
		M44F childFinal = childCuMatrix;

		V3F parentPos = V3F(final(4, 1), final(4, 2), final(4, 3));
		V3F childPos = V3F(childFinal(4, 1), childFinal(4, 2), childFinal(4, 3));
		V3F between = childPos - parentPos;

		float length = between.Length() * 0.5f;

		V3F forward = between;

		between * 0.5f;
		between = parentPos + between;

		V3F worldUp(0, 1, 0);
		V3F right = forward.Cross(-worldUp);
		V3F up = -right.Cross(forward);

		if (right.LengthSqr() < 0.5f)
		{
			right = V3F(0, 0, 1);
			up = V3F(1, 0, 0);
		}

		up.Normalize();
		right.Normalize();
		forward.Normalize();

		CommonUtilities::Matrix4x4<float> mat;

		mat(1, 1) = right.x;
		mat(1, 2) = right.y;
		mat(1, 3) = right.z;
		mat(2, 1) = up.x;
		mat(2, 2) = up.y;
		mat(2, 3) = up.z;
		mat(3, 1) = forward.x;
		mat(3, 2) = forward.y;
		mat(3, 3) = forward.z;
		mat(4, 1) = between.x;
		mat(4, 2) = between.y;
		mat(4, 3) = between.z;
		mat(4, 4) = 1;

		M44F betweenTransform = mat;

		linkPose = MatToTransform(betweenTransform);

		int parentLinkID = originID;

		PxVec3 parentAttachmentFrameOffset = PxVec3(0.0f, 0.0f, 0.0f);
		PxVec3 childAttachmentFrameOffset = PxVec3(0.0f, 0.0f, 0.0f);

		PxMat44 childFrameOffsetMat = MatToTransform(childFinal);
		PxMat44 parenFrameOffsettMat = MatToTransform(final);

		bool isCapsule = true;

		switch (targetID)
		{
			//hiproot	-> spine
		case 2:
			parentAttachmentFrameOffset.x = 0;
			childAttachmentFrameOffset.x = length;

			break;

			//spine		-> SPINE1
		case 3:
			//SPINE1	-> SPINE2
		case 4:
			//SPINE2	-> SPINE3
		case 5:
			//SPINE1	-> Neck
		case 6:
			//Neck		-> Head
		case 7:
			//head		-> Headend
		case 8:
			parentAttachmentFrameOffset.x = length;
			childAttachmentFrameOffset.x = length;
			break;

			//LEFTARM ---------------

			//Spine3	-> Shoulder
		case 21:
			continue;
			break;
			//Shoulder	-> UpperArm
		case 22:
			parentLinkID = 6;
			parentAttachmentFrameOffset.x = length;
			childAttachmentFrameOffset.x = length;
			break;

			//UpperArm	-> LowerArm
		case 23:
			//LowerArm	-> Hand
		case 24:
			//Hand		-> Finger
		case 30:
			parentAttachmentFrameOffset.x = length;
			childAttachmentFrameOffset.x = length;
			break;

			//RIGHT ARM ---------------

			//Spine3	-> Shoulder
		case 34:
			continue;
			break;
			//Shoulder	-> UpperArm
		case 35:
			parentLinkID = 6;
			parentAttachmentFrameOffset.x = length;
			childAttachmentFrameOffset.x = length;
			break;

			//UpperArm	-> LowerArm
		case 36:
			//LowerArm	-> Hand
		case 37:
			//Hand		-> Finger
		case 43:
			parentAttachmentFrameOffset.x = length;
			childAttachmentFrameOffset.x = length;
			break;

			//LEFT LEG	------------------

			//Hiproot	-> Hip
		case 47:
			continue;
			break;
			//Hip		-> LowerLeg
		case 48:
			parentLinkID = 2;
			parentAttachmentFrameOffset.x = 0;
			childAttachmentFrameOffset.x = length;
			break;

			//LowerLeg	-> Foot
		case 49:
			//Foot		-> Toe
		case 50:
			//Toe		-> ToeEnd
		case 51:
			parentAttachmentFrameOffset.x = length;
			childAttachmentFrameOffset.x = length;
			break;

			//Right LEG	------------------

			//Hiproot	-> Hip
		case 52:
			continue;
			break;
			//Hip		-> LowerLeg
		case 53:
			parentLinkID = 2;
			parentAttachmentFrameOffset.x = 0;
			childAttachmentFrameOffset.x = length;
			break;

			//LowerLeg	-> Foot
		case 54:
			//Foot		-> Toe
		case 55:
			//Toe		-> ToeEnd
		case 56:
			parentAttachmentFrameOffset.x = length;
			childAttachmentFrameOffset.x = length;
			break;

		default:
			SYSERROR("CRAZY TARGETNODE in hitboxes create articulation", "");
			break;
		}

		if (createdLinks[parentLinkID] != nullptr)
		{
			parent = createdLinks[parentLinkID];
			physx::PxArticulationLink* newLink;
			newLink = articulation->createLink(parent, linkPose);
			physx::PxArticulationJoint* joint;
			joint = CAST(PxArticulationJoint*, newLink->getInboundJoint());
			joint->setStiffness(10.0f);
			joint->setSwingLimitEnabled(true);

			parenFrameOffsettMat.setPosition(parentAttachmentFrameOffset);
			childFrameOffsetMat.setPosition(-childAttachmentFrameOffset);

			{
				joint->setParentPose(PxTransform(parenFrameOffsettMat));
				joint->setChildPose(PxTransform(childFrameOffsetMat));
				joint->setInternalCompliance(1.0f);
				joint->setExternalCompliance(1.0f);
			}
			
			// GET CHILD AND PARENT POS---------------------------------------------------------------------------
			PxShape* linkshape;
			if (isCapsule)
			{
				(*linkGeometry).radius = 6.0f;
				(*linkGeometry).halfHeight = MAX(1.0f, length - 6.0f);
				linkshape = PxRigidActorExt::createExclusiveShape(*newLink, *linkGeometry, *gMaterial);
			}
			else
			{
				linkshape = PxRigidActorExt::createExclusiveShape(*newLink, *sphereLinkGeometry, *gMaterial);
			}

			physx::PxFilterData filterData;
			filterData.word0 = GBPhysXFilters::CollisionFilter::Enemy;
			filterData.word1 = GBPhysXFilters::CollisionFilter::EnvironmentStatic | GBPhysXFilters::CollisionFilter::EnvironmentDynamic | GBPhysXFilters::CollisionFilter::Player;
			filterData.word3 = 1;
			linkshape->setSimulationFilterData(filterData);
			linkshape->setQueryFilterData(filterData);
			PxRigidBodyExt::updateMassAndInertia(*newLink, 0.01f);

			int toBeCreatedLinkID = sortedHitBox.GetHitBox().myTargetNodeIndex;
			createdLinks[toBeCreatedLinkID] = newLink;

#ifdef _DEBUG
			createdLinksName[toBeCreatedLinkID] = sortedHitBox.GetHitBox().myName;
			createdLinksPositions[toBeCreatedLinkID] = V3F(linkPose.p.x, linkPose.p.y, linkPose.p.z);
#endif // _DEBUG
		}
		else
		{
			SYSERROR("PARENT LINK NOT CREATED IN ARTICULATION", "ragdollerror");
		}
	}

	gScene->addArticulation(*articulation);

	articulation->userData = aCharacter->GetEntity();

	auto force = aDirection;
	force = aDirection * 2000.0f;
	if (force.y < 0)
	{
		force.y *= -1;
	}
	force.y += 1000.0f;

	for (auto& linkk : createdLinks)
	{
		if (linkk != nullptr)
		{
			auto pose = linkk->getGlobalPose();
			M44F rotationMat = M44F::CreateRotationAroundZ(-PI * 0.5f);
			auto rotationTrans = MatToTransform(rotationMat);
			pose = rotationTrans * pose;
			auto charmat = MatToTransform(characterMatrix);
			auto debugtest = charmat * pose;
			linkk->setGlobalPose(debugtest);
			linkk->addForce(PxVec3(force.x, force.y, force.z), physx::PxForceMode::eIMPULSE);
		}
	}

	aCharacter->SetLinkToLookAt(createdLinks);

	return articulation;
}



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//--------------------- OUR GB PHYSX FUNCTIONS --------------------------------
//-----------------------------------------------------------------------------
//---------------------------- (V)òwEV) --------------------------------------
//-----------------------------------------------------------------------------

GBPhysX::~GBPhysX()
{
	SAFE_DELETE(myStaticMeshCooker);
}

GBPhysXActor* GBPhysX::GBCreateDynamicSphere(M44F aMatrixTransform, int aRadius, float aDensity)
{
	physx::PxRigidDynamic* actor;
	actor = createDynamic(PxTransform(PxVec3()), PxSphereGeometry(physx::PxReal(aRadius)), PxVec3(0.0f, 0.0f, 0.0f), aDensity);

	GBPhysXActor* gbActor = new GBPhysXActor();
	gbActor->myGBPhysX = this;
	gbActor->myRigidActor = actor;

	PxShape* p;
	actor->getShapes(&p, 1);
	PxFilterData filterData = PxFilterData();
	PxQueryFilterData queryfilterData = PxQueryFilterData();

	filterData.word0 = GBPhysXFilters::CollisionFilter::EnvironmentDynamic;
	filterData.word1 = GBPhysXFilters::CollisionFilter::EnvironmentStatic | GBPhysXFilters::CollisionFilter::NavMesh;
	filterData.word3 = 1;
	queryfilterData.data = filterData;
	queryfilterData.flags = physx::PxQueryFlag::ePREFILTER | physx::PxQueryFlag::ePOSTFILTER | physx::PxQueryFlag::eSTATIC | physx::PxQueryFlag::eDYNAMIC;

	p->setSimulationFilterData(filterData);
	p->setQueryFilterData(filterData);

	return gbActor;
}

GBPhysXActor* GBPhysX::GBCreateDynamicBox(V3F aPosition, V3F aSize, V3F aForce, float aDensity = 10.0f)
{
	physx::PxRigidDynamic* actor;
	actor = createDynamic(PxTransform(PxVec3(aPosition.x, aPosition.y, aPosition.z)), PxBoxGeometry(aSize.x, aSize.y, aSize.z), PxVec3(aForce.x, aForce.y, aForce.z), aDensity);

	GBPhysXActor* gbActor = new GBPhysXActor();
	gbActor->myGBPhysX = this;
	gbActor->myRigidActor = actor;

	PxShape* shape;
	actor->getShapes(&shape, 1);
	PxFilterData filterData = PxFilterData();
	PxQueryFilterData queryfilterData = PxQueryFilterData();

	filterData.word0 = GBPhysXFilters::CollisionFilter::EnvironmentDynamic;
	filterData.word1 = GBPhysXFilters::CollisionFilter::EnvironmentStatic | GBPhysXFilters::CollisionFilter::NavMesh;
	filterData.word3 = 1;
	queryfilterData.data = filterData;
	queryfilterData.flags = physx::PxQueryFlag::ePREFILTER | physx::PxQueryFlag::ePOSTFILTER | physx::PxQueryFlag::eSTATIC | physx::PxQueryFlag::eDYNAMIC;

	shape->setSimulationFilterData(filterData);
	shape->setQueryFilterData(filterData);

	return gbActor;
}

GBPhysXActor* GBPhysX::GBCreateStaticCube(V3F aPosition, float aHalfSize)
{
	auto actor = createStaticCube(PxTransform(aPosition.x, aPosition.y, aPosition.z), PxReal(aHalfSize));
	GBPhysXActor* gbActor = new GBPhysXActor();
	gbActor->myGBPhysX = this;
	gbActor->myRigidActor = actor;
	return gbActor;
}

GBPhysXActor* GBPhysX::GBCreatePlayerBlockBox(V3F aPosition, V3F aSize, V3F aRotation)
{
	M44F transform = M44F();
	const float degToRad = 57.2957f;

	transform *= transform.CreateRotationAroundX(aRotation.x / degToRad);
	transform *= transform.CreateRotationAroundY(aRotation.y / degToRad);
	transform *= transform.CreateRotationAroundZ(aRotation.z / degToRad);
	transform(4, 1) = aPosition.x;
	transform(4, 2) = aPosition.y;
	transform(4, 3) = aPosition.z;
	float floatz[16] = { transform(1, 1), transform(1, 2), transform(1, 3), transform(1, 4), transform(2, 1), transform(2, 2), transform(2, 3), transform(2, 4), transform(3, 1), transform(3, 2), transform(3, 3), transform(3, 4), transform(4, 1), transform(4, 2), transform(4, 3), transform(4, 4) };
	PxMat44 mat = PxMat44(floatz);
	auto actor = createStaticBox(PxTransform(mat), PxVec3(aSize.x / 2.0f, aSize.y / 2.0f, aSize.z / 2.0f));
	PxShape* shape;
	actor->getShapes(&shape, 1);

	PxFilterData filterData;
	filterData.word0 = GBPhysXFilters::CollisionFilter::EnvironmentMoveBlock;
	filterData.word1 = GBPhysXFilters::CollisionFilter::Player;
	filterData.word3 = 1;
	shape->setSimulationFilterData(filterData);
	shape->setQueryFilterData(filterData);

	GBPhysXActor* gbActor = new GBPhysXActor();
	gbActor->myGBPhysX = this;
	gbActor->myRigidActor = actor;
	return gbActor;
}

GBPhysXActor* GBPhysX::GBCreateKinematicBox(V3F aPosition, V3F aSize, V3F aForce, float aDensity)
{
	physx::PxRigidDynamic* actor;
	actor = createDynamic(PxTransform(PxVec3(aPosition.x, aPosition.y, aPosition.z)), PxBoxGeometry(aSize.x, aSize.y, aSize.z), PxVec3(aForce.x, aForce.y, aForce.z), aDensity);
	actor->setRigidBodyFlags(physx::PxRigidBodyFlag::eKINEMATIC);

	GBPhysXActor* gbActor = new GBPhysXActor();
	gbActor->myGBPhysX = this;
	gbActor->myRigidActor = actor;
	return gbActor;
}

std::vector<GBPhysXActor*> GBPhysX::GBCreateChain(V3F aPosition, V3F aSize, int aLength, float aSeparation)
{
	std::vector<GBPhysXActor*> vector;
	PxMat44 transform = PxMat44(PxVec4(1.0f, 0.0f, 0.0f, 0.0f), PxVec4(0.0f, 1.0f, 0.0f, 0.0f), PxVec4(0.0f, 0.0f, 1.0f, 0.0f), PxVec4(aPosition.x, aPosition.y, aPosition.z, 1.0f));
	std::vector<PxRigidDynamic*> dynamics = createChain(PxTransform(transform), aLength, physx::PxBoxGeometry(aSize.x, aSize.y, aSize.z), aSeparation, createDampedD6);
	for (auto& dynamic : dynamics)
	{
		GBPhysXActor* gbActor = new GBPhysXActor();
		gbActor->myGBPhysX = this;
		gbActor->myRigidActor = dynamic;
		vector.push_back(gbActor);
	}
	return vector;
}

//-------------------------------------------------------------------------------
//-----------------------  SPECIALS  --------------------------------------------
//-------------------------------------------------------------------------------

PxController* CreateCharacterController(V3F aPosition, PxFilterData* aFilterData)
{
	PxCapsuleControllerDesc desc;

	desc.material = gMaterial;
	desc.position = PxExtendedVec3(aPosition.x, aPosition.y, aPosition.z);
	desc.height = PlayerHeight;
	desc.radius = PlayerRadius;
	desc.slopeLimit = 0.0f;
	desc.contactOffset = 0.1f;
	desc.stepOffset = 0.02f;
	desc.density = 10.0f;
	desc.reportCallback = new GBPhysXControllerHitReportCallback();

	PxController* controller = manager->createController(desc);

	auto actor = controller->getActor();
	physx::PxShape* shape;
	actor->getShapes(&shape, 1);
	actor->setContactReportThreshold(0);

	shape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, true);
	shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);
	shape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);
	shape->setSimulationFilterData(*aFilterData);
	shape->setQueryFilterData(*aFilterData);

	return controller;
}

std::vector<GBPhysXActor*> GBPhysX::GBCreateHangingLantern(V3F aPosition)
{
	std::vector<GBPhysXActor*> returnVector;

	auto chainVector = GBCreateChain(aPosition, V3F(5.0f, 5.0f, 5.0f), 3, 10.0f);
	returnVector.push_back(chainVector[0]);
	returnVector.push_back(chainVector[1]);
	returnVector.push_back(chainVector[2]);

	PxRigidDynamic* prev = (PxRigidDynamic*)chainVector[chainVector.size() - 1]->myRigidActor;
	PxVec3 offset(20.0f / 2, 0, 0);
	PxTransform localTm(offset);

	PxRigidDynamic* lamp = PxCreateDynamic(*gPhysics, prev->getGlobalPose() * localTm, PxBoxGeometry(25.0f, 10.0f, 10.0f), *gMaterial, 10.0f);
	createDampedD6(prev, PxTransform(offset), lamp, PxTransform(-offset));
	gScene->addActor(*lamp);

	GBPhysXActor* gbLampActor = new GBPhysXActor();
	gbLampActor->myGBPhysX = this;
	gbLampActor->myRigidActor = lamp;
	returnVector.push_back(gbLampActor);

	return returnVector;
}

std::array<GBPhysXActor*, 8> GBPhysX::GBCreateBarrelDestructable(V3F aPosition, V3F aRotation)
{
	std::array<GBPhysXActor*, 8> returnArray;
	V3F dir = aRotation.GetNormalized();
	dir = dir * 600.0f;
	//LID fbx1
	returnArray[0] = GBCreateDynamicBox(aPosition + V3F(-2.0f, 116.0f, 2.8f), V3F(33.0f, 4.0f, 33.0f), dir, 10.0f);
	//doublepiece fbx2
	returnArray[1] = GBCreateDynamicBox(aPosition + V3F(10.80f, 52.1f, -36.0f), V3F(18.0f, 48.681f, 6.208f), dir, 10.0f);
	//singlepiece fbx3
	returnArray[2] = GBCreateDynamicBox(aPosition + V3F(38.624f, 55.444f, -13.886f), V3F(5.0f, 48.681f, 9.892f), dir, 10.0f);
	//singlepiece fbx4
	returnArray[3] = GBCreateDynamicBox(aPosition + V3F(40.409f, 55.351f, 17.715f), V3F(4.0f, 48.681f, 10.338f), dir, 10.0f);
	//Singlepiece fbx5
	returnArray[4] = GBCreateDynamicBox(aPosition + V3F(27.197f, 55.647f, 39.977f), V3F(7.342f, 48.681f, 5.307f), dir, 10.0f);
	//doublepiece fbx6
	returnArray[5] = GBCreateDynamicBox(aPosition + V3F(-36.082f, 53.470f, -25.616f), V3F(10.517f, 48.681f, 12.589f), dir, 10.0f);
	//doublepiece fbx7
	returnArray[6] = GBCreateDynamicBox(aPosition + V3F(-36.848f, 55.127f, 28.365f), V3F(10.517f, 48.681f, 17.692f), dir, 10.0f);
	//singlepiece fbx8
	returnArray[7] = GBCreateDynamicBox(aPosition + V3F(-3.167f, 54.744f, 45.588f), V3F(10.517f, 48.681f, 7.461f), dir, 10.0f);

	return returnArray;
}

std::array<GBPhysXActor*, 8> GBPhysX::GBCreateBoxDestructable(V3F aPosition, V3F aRotation)
{
	std::array<GBPhysXActor*, 8> returnArray;
	V3F dir = aRotation.GetNormalized();
	dir = dir * 600.0f;
	//HEL SIDA fbx 1
	returnArray[0] = GBCreateDynamicBox(aPosition + V3F(45.69f, 42.08f, 0.784f), V3F(7.72f, 31.0f, 43.0f), dir, 10.0f);

	//BOTTEN fbx 2
	returnArray[1] = GBCreateDynamicBox(aPosition + V3F(0.0f, 0.0f, 0.0f), V3F(53.0f, 4.0f, 58.0f), dir, 10.0f);

	//LOCK BIT ETT OCH TVÅ fbx 3
	returnArray[2] = GBCreateDynamicBox(aPosition + V3F(-3.45f, 90.45f, -27.08f), V3F(53.0f, 4.5f, 28.0f), dir, 10.0f);
	returnArray[3] = GBCreateDynamicBox(aPosition + V3F(-3.45f, 90.45f, 35.70f), V3F(53.0f, 4.5f, 28.0f), dir, 10.0f);

	// PLANKA SIDA ETT TVÅ TRE fbx 5
	returnArray[4] = GBCreateDynamicBox(aPosition + V3F(-1.24f, 65.06f, 45.88f), V3F(33.5f, 8.0f, 4.0f), dir, 10.0f);
	returnArray[5] = GBCreateDynamicBox(aPosition + V3F(-1.24f, 42.06f, 45.88f), V3F(33.5f, 8.0f, 4.0f), dir, 10.0f);
	returnArray[6] = GBCreateDynamicBox(aPosition + V3F(-1.24f, 18.06f, 45.88f), V3F(33.5f, 8.0f, 4.0f), dir, 10.0f);

	// HÖRNBIT fbx 6
	returnArray[7] = GBCreateDynamicBox(aPosition + V3F(-50.65f, 46.95f, 48.82f), V3F(8.0f, 37.0f, 10.0f), dir, 10.0f);

	return returnArray;
}

GBPhysXCharacter* GBPhysX::GBCreateCapsuleController(V3F aPosition, V3F aRotation, float aHeight, float aRadius, bool aIsPlayer)
{
	physx::PxRigidDynamic* actor;
	physx::PxController* controller;

	GBPhysXCharacter* gbChar = new GBPhysXCharacter();
	gbChar->Init(aIsPlayer);

	controller = CreateCharacterController(aPosition, gbChar->myFilterData);
	actor = controller->getActor();

	gbChar->myController = controller;
	gbChar->myGBPhysX = this;
	gbChar->myRigidActor = actor;

	if (aIsPlayer)
	{
		actor->setName("PlayerCapsule");
	}
	else
	{
		actor->setName("EnemyCapsule");
	}

	controller->setUserData(gbChar);
	controller->getActor()->userData = gbChar;
	return gbChar;
}

BulletHitReport GBPhysX::RayPickActor(V3F aOrigin, V3F aDirection, bool aIsBullet)
{
	V3F slightlyForward = aOrigin + aDirection * 50.0f;
	PxVec3 origin = PxVec3(slightlyForward.x, slightlyForward.y, slightlyForward.z);
	PxVec3 dir = PxVec3(aDirection.x, aDirection.y, aDirection.z);
	PxRaycastBuffer buffer;
	PxFilterData filterData = PxFilterData();
	PxQueryFilterData queryfilterData = PxQueryFilterData();
	if (aIsBullet)
	{
		filterData.word0 = GBPhysXFilters::CollisionFilter::Enemy | GBPhysXFilters::CollisionFilter::EnvironmentDynamic | GBPhysXFilters::CollisionFilter::EnvironmentStatic;
		queryfilterData.data = filterData;
		queryfilterData.flags = physx::PxQueryFlag::ePREFILTER | physx::PxQueryFlag::ePOSTFILTER | physx::PxQueryFlag::eSTATIC | physx::PxQueryFlag::eDYNAMIC;

		gScene->raycast(origin, dir, 6000, buffer, physx::PxHitFlag::eDEFAULT, queryfilterData, bulletQueryFilterCallback);
	}
	else
	{
		filterData.word0 = GBPhysXFilters::CollisionFilter::EnvironmentStatic;
		queryfilterData.data = filterData;
		queryfilterData.flags = physx::PxQueryFlag::ePREFILTER | physx::PxQueryFlag::ePOSTFILTER | physx::PxQueryFlag::eSTATIC | physx::PxQueryFlag::eDYNAMIC;
		gScene->raycast(origin, dir, 150, buffer, physx::PxHitFlag::eDEFAULT, queryfilterData, interactQueryFilterCallback);
	}

	BulletHitReport report;
	if (buffer.hasBlock)
	{
		GBPhysXActor* hitActor = (GBPhysXActor*)buffer.block.actor->userData;
		PxVec3 worldSpaceHitPos = buffer.block.position;
		PxVec3 worldSpaceHitNormal = buffer.block.normal;

		report.actor = hitActor;
		report.position = V3F(worldSpaceHitPos.x, worldSpaceHitPos.y, worldSpaceHitPos.z);
		report.normal = V3F(worldSpaceHitNormal.x, worldSpaceHitNormal.y, worldSpaceHitNormal.z);
	}
	else
	{
		report.actor = nullptr;
		report.position = V3F();
		report.normal = V3F();
	}
	report.incomingAngle = aDirection;

	return report;
}

void GBPhysX::GBCreateNavMesh(int aNumberOfVerts, std::vector<V3F> someVerts, int aNumberOfTriangles, std::vector<int> someIndices)
{

	PxU32 aNumVertices = aNumberOfVerts;

	PxVec3* someVertices = new PxVec3[aNumVertices];
	for (int i = 0; i < aNumberOfVerts; i++)
	{
		someVertices[i] = PxVec3(someVerts[i].x, someVerts[i].y, someVerts[i].z);
	}

	PxU32 aNumTriangles = aNumberOfTriangles;

	PxU32 nrOfIndices = static_cast<PxU32>(someIndices.size());
	PxU32* Indices = new PxU32[nrOfIndices];
	for (size_t i = 0; i < nrOfIndices; i++)
	{
		Indices[i] = someIndices[i];
	}

	createStaticNavMesh(aNumVertices, someVertices, aNumTriangles, Indices);
}

void GBPhysX::GBInitPhysics(bool aInteractive)
{
	initPhysics(aInteractive);
	SetGBPhysXActive(true);

	myStaticMeshCooker = new StaticMeshCooker();
	myStaticMeshCooker->Init(gFoundation, gPhysics);
}

void GBPhysX::GBStepPhysics(float aDeltaTime)
{
	stepPhysics(aDeltaTime);
}

void GBPhysX::GBCleanUpPhysics()
{
	myStaticMeshCooker->CleanUp();
	SAFE_DELETE(myStaticMeshCooker);
	cleanupPhysics(false);
	SetGBPhysXActive(false);
}

bool GBPhysX::GetGBPhysXActive()
{
	return myIsActive;
}

void GBPhysX::GBSetKinematicActorTargetPos(physx::PxRigidActor* aActor, V3F aPosition, M44F aRotation)
{
	float bob[16]{ aRotation(1, 1), aRotation(1, 2), aRotation(1, 3), aRotation(1, 4),
					aRotation(2, 1), aRotation(2, 2), aRotation(2, 3), aRotation(2, 4),
					aRotation(3, 1), aRotation(3, 2), aRotation(3, 3), aRotation(3, 4),
					aPosition.x, aPosition.y, aPosition.z, 1.0f };
	PxTransform trans = PxTransform(PxMat44(bob));

	setKinematicActorTargetPos(aActor, trans);
}

void GBPhysX::GBApplyForceToActor(physx::PxRigidActor* aActor, V3F aForce)
{
	PxVec3 vec = PxVec3(aForce.x, aForce.y, aForce.z);
	addForceToDynamicActor(aActor, vec);
}

void GBPhysX::GBResetScene()
{
	const unsigned int bufferSize = 512;
	PxActor* ActorArray[bufferSize];
	unsigned int numberOfActors = 0;

	do
	{
		numberOfActors = gScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC, ActorArray, bufferSize, 0);

		gScene->removeActors(ActorArray, numberOfActors);
	} while (numberOfActors == bufferSize);

	do
	{
		numberOfActors = gScene->getActors(PxActorTypeFlag::eRIGID_STATIC, ActorArray, bufferSize, 0);

		gScene->removeActors(ActorArray, numberOfActors);
	} while (numberOfActors == bufferSize);
}

void GBPhysX::RemoveActor(physx::PxRigidActor* aRigidActor)
{
	gScene->removeActor(*aRigidActor);
}

void GBPhysX::ReleaseActor(physx::PxRigidActor* aRigidActor)
{
	//aRigidActor->release();
}

GBPhysXActor* GBPhysX::CreateStaticTriangleMeshObject(std::string& aFilePath, M44F aTransform)
{
	if (myStaticMeshCooker->TriangleMeshExists(aFilePath))
	{
		PxTriangleMesh* tMesh = myStaticMeshCooker->GetTriangleMeshFromPath(aFilePath);
	
		float transform[16] = { aTransform(1,1),aTransform(1,2),aTransform(1,3),aTransform(1,4),aTransform(2,1),aTransform(2,2),aTransform(2,3),aTransform(2,4),aTransform(3,1),aTransform(3,2),aTransform(3,3),aTransform(3,4), aTransform(4,1), aTransform(4,2),aTransform(4,3),1.0f };
		PxRigidStatic* body = PxCreateStatic(*gPhysics, PxTransform(PxMat44(transform)), PxTriangleMeshGeometry(tMesh), *gMaterial);

		if (!body)
		{
			SYSERROR("failed creating mesh object for mesh", aFilePath);
			return nullptr;
		}
		gScene->addActor(*body);

		physx::PxShape* shape;
		body->getShapes(&shape, 1);
		body->setName("static object");

		physx::PxFilterData filterData;
		filterData.word0 = GBPhysXFilters::CollisionFilter::EnvironmentStatic;
		filterData.word1 = GBPhysXFilters::CollisionFilter::Player | GBPhysXFilters::CollisionFilter::Enemy | GBPhysXFilters::CollisionFilter::EnvironmentDynamic;
		filterData.word3 = 1;

		shape->setSimulationFilterData(filterData);
		shape->setQueryFilterData(filterData);


		GBPhysXActor* gbActor = new GBPhysXActor();
		gbActor->myGBPhysX = this;
		gbActor->myRigidActor = body;
		body->userData = gbActor;
		return gbActor;
	}
	return nullptr;
}

bool GBPhysX::CookStaticTriangleMesh(std::string& aFilePath, const aiScene* aAiScene, aiNode* aAiNode)
{
	if (myStaticMeshCooker->TriangleMeshExists(aFilePath))
	{
		SYSINFO("loaded: " + aFilePath + " from precooked data!!");
		return true;
	}
	if (myStaticMeshCooker->LoadCollisionMesh(gPhysics, aFilePath, aAiScene, aAiNode))
	{
		return true;
	}
	return false;
}

void GBPhysX::SetZombieHitBoxData(std::vector<HitBox>& aHitBoxes)
{
	myZombieHitBoxes = aHitBoxes;
}

std::vector<HitBox>& GBPhysX::GetZombieHitBoxes()
{
	return myZombieHitBoxes;
}

void GBPhysX::SaveNodeParents(std::unordered_map<int, int> nodeParents)
{
	myRagdollNodeParents = nodeParents;
}

void GBPhysX::GBCreateRagDoll(V3F aPos, GBPhysXCharacter* aChar, V3F aDirection)
{

	PxVec3 worldPos = PxVec3(aPos.x, aPos.y, aPos.z);

	aChar->SetArticulation(CreateArticulation(worldPos, aChar, aChar->myHitBoxes, myRagdollNodeParents, aDirection));
	aChar->SetIsRagDoll(true);
	aChar->GetEntity()->GetComponent<Mesh>()->GetModelInstance()->SetGBPhysXCharacter(aChar);
}

void GBPhysX::SetGBPhysXActive(bool aActive)
{
	myIsActive = aActive;
}

GBPhysXActor::GBPhysXActor()
{
}

GBPhysXActor::~GBPhysXActor()
{
}

M44F GBPhysXActor::GetTransformMatrix()
{
	auto mat = PxMat44(myRigidActor->getGlobalPose());
	auto mat4 = M44F(mat.column0.x, mat.column0.y, mat.column0.z, mat.column0.w, mat.column1.x, mat.column1.y, mat.column1.z, mat.column1.w, mat.column2.x, mat.column2.y, mat.column2.z, mat.column2.w, mat.column3.x, mat.column3.y, mat.column3.z, mat.column3.w);
	return mat4;
}

V3F GBPhysXActor::GetPosition()
{
	auto mat = GetTransformMatrix();
	V3F pos = V3F(mat(4, 1), mat(4, 2), mat(4, 3));
	return pos;
}

void GBPhysXActor::RemoveFromScene()
{
	if (!myActorHasBeenReleased)
	{
		if (myRigidActor)
		{
			myGBPhysX->RemoveActor(myRigidActor);
		}
	}
}

void GBPhysXActor::Release()
{
	if (myRigidActor)
	{
		myGBPhysX->ReleaseActor(myRigidActor);
	}
}

void GBPhysXActor::SetIsKinematic(bool aIsKinematic)
{
	myIsKinematic = aIsKinematic;
}

void GBPhysXActor::SetKinematicTargetPos(V3F aPos)
{
	if (myIsKinematic)
	{
		myGBPhysX->GBSetKinematicActorTargetPos(myRigidActor, aPos, M44F());
	}
	else
	{
		SYSERROR("Tried setting kinematic target pos of non kinematic rigid actor", "");
	}
}

void GBPhysXActor::ApplyForce(V3F aDirection, float aMagnitude)
{
	if (myRigidActor)
	{
		myGBPhysX->GBApplyForceToActor(myRigidActor, aDirection * aMagnitude);
	}
}

void GBPhysXActor::SleepActor()
{
	myRigidActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);
}

void GBPhysXActor::SetGBPhysXPtr(GBPhysX* aGBPhysXPtr)
{
	myGBPhysX = aGBPhysXPtr;
}

void GBPhysXActor::SetEntity(Entity* aEntity)
{
	myEntity = aEntity;
}

Entity* GBPhysXActor::GetEntity()
{
	if (myEntity)
	{
		return myEntity;
	}
	else
	{
		return nullptr;
	}
}

void GBPhysXActor::SetShapeName(const std::string& aName)
{
	physx::PxShape* buffer[128];
	int count = myRigidActor->getShapes(buffer, 128, 0);
	for (size_t i = 0; i < count; i++)
	{
		buffer[i]->setName(aName.c_str());
	}
}

void GBPhysXActor::SetRigidActor(physx::PxRigidActor* aRigidActor)
{
	myRigidActor = aRigidActor;
}

physx::PxRigidActor* GBPhysXActor::GetRigidActor()
{
	return myRigidActor;
}

void GBPhysXActor::SetIsHitBox(bool aIsHitBox)
{
	myIsHitBox = aIsHitBox;
}

bool GBPhysXActor::GetIsHitBox()
{
	return myIsHitBox;
}

bool GBPhysXActor::GetHasBeenReleased()
{
	return myActorHasBeenReleased;
}

std::string GBPhysXActor::GetName()
{
	return myRigidActor->getName();
}

void GBPhysXCharacter::SetIsRagDoll(bool aIsRagDoll)
{
	myIsRagDoll = aIsRagDoll;
}

bool GBPhysXCharacter::GetIsRagDoll()
{
	return myIsRagDoll;
}

void GBPhysXCharacter::SetArticulation(physx::PxArticulation* aArticulation)
{
	myArticulation = aArticulation;
}

void GBPhysXCharacter::SetLinkToLookAt(std::array<physx::PxArticulationLink*, 64>& aLinkToLookAtArray)
{
	myLinkToLookAt = aLinkToLookAtArray;
}

void GBPhysXCharacter::SetDeathMomentBoneTransformations(std::array<CommonUtilities::Matrix4x4<float>, 64> aMatrixes)
{
	myDeathMomentBoneTransformations = aMatrixes;
}

void GBPhysXCharacter::SetDeathMomentCharTransform(M44F& aTransform)
{
	myDeathMomentCharTransform = aTransform;
}

void GBPhysXCharacter::UpdateRagDollMatrices(std::array<CommonUtilities::Matrix4x4<float>, 64>& aMatrixes)
{
	static float currentTestXRotationOffset = 0.0f;

	if (myIsRagDoll == true)
	{
		auto nodeParents = myGBPhysX->myRagdollNodeParents;

		auto listoflinks = myLinkToLookAt;
		listoflinks[25] = listoflinks[30];
		listoflinks[38] = listoflinks[43];



		int size = myArticulation->getNbLinks();
		PxArticulationLink* buffer[64];
		int nrOfWrittenToBuffer = myArticulation->getLinks(buffer, 64, 0);
		PxTransform linkTransform;

		auto boneInfo = GetEntity()->GetComponent<AnimationComponent>()->GetAnimator()->GetBoneInfo();

		//DEBUG SANITY CHECKS
		PxTransform deathMomentTransform = MatToTransform(myDeathMomentCharTransform);
		PxTransform inverseCharTransform = MatToTransform(myDeathMomentCharTransform).getInverse();
		PxTransform debugLinkTransform = listoflinks[2]->getGlobalPose();
		M44F debugBoneoffset = (*boneInfo)[1].BoneOffset;
		M44F debugBoneoffsetinverse = M44F::GetRealInverse(debugBoneoffset);
		PxTransform debugBoneTrans = MatToTransform(debugBoneoffset);
		PxTransform debugtemp = debugBoneTrans * debugLinkTransform * inverseCharTransform;


		M44F tempMatrix = TransformToMat(debugtemp);
		M44F finalMat = M44F::GetRealInverse(TransformToMat(debugtemp));
		M44F saveMatrix = M44F();
		for (int index = 0; index < 64; index++)
		{
			if (index == 1)
			{
				linkTransform = listoflinks[2]->getGlobalPose();
				linkTransform = inverseCharTransform * linkTransform;

				M44F linkFinal = TransformToMat(linkTransform);
				M44F linkCuMatrix = M44F::Transpose(linkFinal);


				M44F linkMatrix(linkCuMatrix(1, 1), linkCuMatrix(1, 2), linkCuMatrix(1, 3), linkCuMatrix(1, 4),
					linkCuMatrix(2, 1), linkCuMatrix(2, 2), linkCuMatrix(2, 3), linkCuMatrix(2, 4),
					linkCuMatrix(3, 1), linkCuMatrix(3, 2), linkCuMatrix(3, 3), linkCuMatrix(3, 4),
					linkCuMatrix(4, 1), linkCuMatrix(4, 2), linkCuMatrix(4, 3), linkCuMatrix(4, 4));

				M44F linkBoneOffset = (*boneInfo)[index].BoneOffset;

				M44F linkTrans = linkBoneOffset * linkMatrix;

				//tempMatrix = TransformToMat(linkTransform);

				//PxTransform boneTrans = MatToTransform((*boneInfo)[index].BoneOffset);
				//boneTrans = boneTrans.getInverse();

				//VERSION WITH BONEOFFSET * world
				//finalMat = tempMatrix * (*boneInfo)[index].BoneOffset;
				//PxTransform temp = boneTrans * linkTransform * inverseCharTransform;
				//finalMat = TransformToMat(temp);
				//finalMat = M44F::GetRealInverse(finalMat);
				aMatrixes[index] = linkTrans;
				//aMatrixes[index] = finalMat;
				//saveMatrix = tempMatrix;

				aMatrixes[index] = myDeathMomentBoneTransformations[index];
			}
			else if (index > 1)
			{
				linkTransform = listoflinks[(nodeParents[index] + 1)]->getGlobalPose();
				linkTransform = inverseCharTransform * linkTransform;
				tempMatrix = TransformToMat(linkTransform);

				//VERSION WITH BONEOFFSET * world
				finalMat = tempMatrix * (*boneInfo)[index].BoneOffset;
				aMatrixes[index] = finalMat;
				//aMatrixes[index] = myDeathMomentBoneTransformations[index];

				//VERSION WITH JUST REMOVED DEATHMOMENT
				//M44F deathMomentMat = myDeathMomentBoneTransformations[index];
				//M44F deathMomentInverseMat = M44F::GetRealInverse(deathMomentMat);
				//
				//M44F finalMat = deathMomentInverseMat * tempMatrix;
				//aMatrixes[index] = finalMat;

				//VERSION WITH REMOVED BONEOFFSET
				//M44F removedBoneOffset = M44F::GetRealInverse((*boneInfo)[index].BoneOffset) * tempMatrix;
				//aMatrixes[index] = removedBoneOffset;
			}
			else
			{
				aMatrixes[index] = myDeathMomentBoneTransformations[index];
			}
		}

	}
}

GBPhysXCharacter::GBPhysXCharacter()
{
	myIsCrouching = false;
	myIsGrounded = false;
	myFilterData = nullptr;
}

GBPhysXCharacter::~GBPhysXCharacter()
{
	PX_RELEASE(myArticulation);
	PX_RELEASE(myController);
	SAFE_DELETE(myFilterData);
}

void GBPhysXCharacter::Init(bool aIsPlayer)
{
	SAFE_DELETE(myFilterData);

	myFilterData = new PxFilterData();
	if (aIsPlayer)
	{
		myFilterData->word0 = GBPhysXFilters::CollisionFilter::Player;
		myFilterData->word1 = GBPhysXFilters::CollisionFilter::Enemy | GBPhysXFilters::CollisionFilter::EnvironmentDynamic | GBPhysXFilters::CollisionFilter::EnvironmentStatic;
	}
	else
	{
		myFilterData->word0 = GBPhysXFilters::CollisionFilter::Enemy;
		myFilterData->word1 = GBPhysXFilters::CollisionFilter::Player | GBPhysXFilters::CollisionFilter::EnvironmentDynamic | GBPhysXFilters::CollisionFilter::EnvironmentStatic | GBPhysXFilters::CollisionFilter::NavMesh;
	}

	myFilterData->word3 = 1;
}

void GBPhysXCharacter::Move(V3F aDirection, float aDeltaTime)
{
	if (myController)
	{
		PxVec3 dir = PxVec3(aDirection.x, aDirection.y + myCurrentFallVelocity, aDirection.z);
		PxF32 minDist = 0.1f;
		PxF32 elapsedTime = aDeltaTime;

		PxControllerFilters filters;
		filters.mFilterFlags = physx::PxQueryFlag::eDYNAMIC | physx::PxQueryFlag::ePREFILTER | physx::PxQueryFlag::eSTATIC | physx::PxQueryFlag::ePOSTFILTER;
		filters.mFilterData = myFilterData;
		filters.mFilterCallback = queryFilterCallback;
		filters.mCCTFilterCallback = cctQueryFilterCallback;
		myController->move(dir, minDist, elapsedTime, filters);

		myCurrentFallVelocity -= 9.82f * aDeltaTime * PlayerGravityModifier;
	}
}

void GBPhysXCharacter::SetIsGrounded(bool aIsGrounded)
{
	myIsGrounded = aIsGrounded;
	myCurrentFallVelocity = 0.0f;
}

bool GBPhysXCharacter::GetIsGrounded() const
{
	return myIsGrounded;
}

void GBPhysXCharacter::Jump()
{
	myCurrentFallVelocity = 9.92f;
}

void GBPhysXCharacter::Crouch()
{
	myController->resize(PxReal(1.0f));
	myIsCrouching = true;
}

void GBPhysXCharacter::Stand()
{
	myController->resize(PxReal(PlayerHeight));
	myIsCrouching = false;
}


bool GBPhysXCharacter::GetIsCrouching() const
{
	return myIsCrouching;
}

void GBPhysXCharacter::Teleport(V3F aPosition)
{
	myController->setPosition(PxExtendedVec3(aPosition.x, aPosition.y, aPosition.z));
}

void GBPhysXCharacter::CreateHitBoxes()
{
	if (myController != nullptr)
	{
		if (myController->getActor()->userData != nullptr)
		{
			GBPhysXCharacter* character = CAST(GBPhysXCharacter*, myController->getActor()->userData);
			Entity* entity = character->GetEntity();
			if (entity->GetEntityType() != EntityType::Player)
			{

				if (entity->GetComponent<AnimationComponent>() != nullptr)
				{
					std::array<M44F, NUMBEROFANIMATIONBONES> boneTransforms;
					Animator* animator = entity->GetComponent<AnimationComponent>()->GetAnimator();
					if (animator)
					{
						animator->BoneTransform(boneTransforms);

						PxExtendedVec3 extendedPos = myController->getPosition();
						M44F finalPos;
						V3F worldPos = V3F(extendedPos.x, extendedPos.y, extendedPos.z);
						finalPos(4, 1) = worldPos.x;
						finalPos(4, 2) = worldPos.y;
						finalPos(4, 3) = worldPos.z;

						std::vector<HitBox> hitBoxes = myGBPhysX->GetZombieHitBoxes();
						myHitBoxes.reserve(64);
						for (auto& hitbox : hitBoxes)
						{
							GBPhysXHitBox* gbHitBox = new GBPhysXHitBox();
							gbHitBox->SetGBPhysXPtr(myGBPhysX);
							gbHitBox->SetEntity(CAST(GBPhysXCharacter*, myController->getUserData())->GetEntity());
							gbHitBox->SetIsHitBox(true);
							gbHitBox->SetHitBox(hitbox);

							HitBoxType type = gbHitBox->GetHitBox().myHitBoxType;
							PxCapsuleGeometry geom;

							geom = GetHitboxGeometry(type);

							//auto aTransform = boneTransforms[hitbox.myOriginNodeIndex];
							auto aTransform = M44F();
							aTransform = M44F::GetRealInverse(aTransform);
							float transform[16] = { aTransform(1,1),aTransform(1,2),aTransform(1,3),aTransform(1,4),aTransform(2,1),aTransform(2,2),aTransform(2,3),aTransform(2,4),aTransform(3,1),aTransform(3,2),aTransform(3,3),aTransform(3,4), aTransform(4,1) + worldPos.x, aTransform(4,2) + worldPos.y,aTransform(4,3) + worldPos.z,1.0f };
							PxMat44 matrix = PxMat44(transform);
							PxShape* shape = gPhysics->createShape(geom, *gMaterial, true);

							physx::PxFilterData filterData;
							filterData.word0 = GBPhysXFilters::CollisionFilter::EnemyHitBox;
							filterData.word1 = GBPhysXFilters::CollisionFilter::Bullet;
							filterData.word3 = 1;

							shape->setName(hitbox.myName.c_str());
							shape->setSimulationFilterData(filterData);
							shape->setQueryFilterData(filterData);

							auto actor = PxCreateKinematic(*gPhysics, PxTransform(matrix), *shape, PxReal(10.0f));
							actor->setName(hitbox.myName.c_str());

							gScene->addActor(*actor);

							actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
							gbHitBox->SetRigidActor(actor);
							myHitBoxes.push_back(gbHitBox);
							actor->userData = gbHitBox;
						}
					}
				}
			}
		}
	}
}
void GBPhysXCharacter::UpdateHitBoxes()
{
	if (myHitBoxes.size() > 0)
	{
		if (myController)
		{
			std::array<M44F, NUMBEROFANIMATIONBONES> boneTransforms;
			GBPhysXCharacter* character = CAST(GBPhysXCharacter*, myController->getActor()->userData);
			Entity* entity = character->GetEntity();
			if (entity)
			{
				Animator* animator = entity->GetComponent<AnimationComponent>()->GetAnimator();
				if (animator)
				{
					animator->BoneTransform(boneTransforms);
					PxTransform traaansform;
					for (auto& currentGBHitbox : myHitBoxes)
					{
						HitBox* currentHitBox = &currentGBHitbox->GetHitBox();
						auto entityPos = myController->getPosition();

						auto& childBoneOffset = currentHitBox->childBoneData.BoneOffset;
						auto& parentBoneOffset = currentHitBox->parentBoneData.BoneOffset;

						auto& childTrans = boneTransforms[currentHitBox->myTargetNodeIndex];
						auto& parentTrans = boneTransforms[currentHitBox->myOriginNodeIndex];

						M44F childMatrix = childTrans * currentHitBox->childBoneOffsetInversed;

						M44F childCuMatrix(childMatrix(1, 1), childMatrix(1, 2), childMatrix(1, 3), childMatrix(1, 4),
							childMatrix(2, 1), childMatrix(2, 2), childMatrix(2, 3), childMatrix(2, 4),
							childMatrix(3, 1), childMatrix(3, 2), childMatrix(3, 3), childMatrix(3, 4),
							childMatrix(4, 1), childMatrix(4, 2), childMatrix(4, 3), childMatrix(4, 4));

						childCuMatrix = M44F::Transpose(childCuMatrix);
						M44F& childFinal = childCuMatrix;

						M44F parentMatrix = parentTrans * currentHitBox->parentBoneOffsetInversed;

						M44F parentCuMatrix(parentMatrix(1, 1), parentMatrix(1, 2), parentMatrix(1, 3), parentMatrix(1, 4),
							parentMatrix(2, 1), parentMatrix(2, 2), parentMatrix(2, 3), parentMatrix(2, 4),
							parentMatrix(3, 1), parentMatrix(3, 2), parentMatrix(3, 3), parentMatrix(3, 4),
							parentMatrix(4, 1), parentMatrix(4, 2), parentMatrix(4, 3), parentMatrix(4, 4));

						parentCuMatrix = M44F::Transpose(parentCuMatrix);
						M44F& parentFinal = parentCuMatrix;


						V3F childWorldPos = { childFinal(4,1), childFinal(4,2), childFinal(4,3) };
						V3F parentWorldPos = { parentFinal(4,1), parentFinal(4,2), parentFinal(4,3) };
						V3F offset = childWorldPos - parentWorldPos;
						V3F halfOffset = offset * 0.5f;
						V4F finalOffset = V4F(halfOffset.x, halfOffset.y, halfOffset.z, 1.0f);


						entityPos.y -= PlayerHeight;
						M44F mat = entity->GetRotation();
						mat *= M44F::CreateRotationAroundY(PI);

						finalOffset = finalOffset * mat;

						mat(4, 1) = entityPos.x;
						mat(4, 2) = entityPos.y;
						mat(4, 3) = entityPos.z;

						mat = parentFinal * mat;
						mat(4, 1) += finalOffset.x;
						mat(4, 2) += finalOffset.y;
						mat(4, 3) += finalOffset.z;

						currentGBHitbox->GetRigidActor()->setGlobalPose(MatToTransform(mat));
					}
				}

			}
		}
	}
	else
	{
		CreateHitBoxes();
	}
}

void GBPhysXCharacter::Kill()
{
	if (!myActorHasBeenReleased)
	{
		ReleaseHitBoxes();
		myGBPhysX->RemoveActor(myRigidActor);
		myActorHasBeenReleased = true;
	}
}

void GBPhysXCharacter::ReleaseHitBoxes()
{
	if (!myActorHasBeenReleased)
	{
		for (int index = myHitBoxes.size() - 1; index >= 0; index--)
		{
			myHitBoxes[index]->RemoveFromScene();
			myHitBoxes[index]->Release();
			myHitBoxes[index] = nullptr;
		}
	}
}

void GBPhysXHitBox::SetHitBox(HitBox aHitBox)
{
	myHitBox = aHitBox;
}

HitBox& GBPhysXHitBox::GetHitBox()
{
	return myHitBox;
}
