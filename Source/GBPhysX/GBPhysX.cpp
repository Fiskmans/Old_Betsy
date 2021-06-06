#include <pch.h>

#include "GBPhysX.h"
#include "SnippetUtils.h"
#include "include\PxPhysicsAPI.h"
#include "include\PxRigidActor.h"
#include "GBPhysXLibraryDependencies.h"
#include "GBPhysXQueryFilterCallback.h"
#include "GBPhysXSimulationEventCallback.h"
#include "GBPhysXSimulationFilterCallback.h"
#include "GBPhysXControllerHitReportCallback.h"
#include "StaticMeshCooker.h"
#include "Animator.h"

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
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
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


physx::PxTransform MatToTransform(const M44f& aMatrix)
{
	float transform[16] = { aMatrix(1,1),aMatrix(1,2),aMatrix(1,3),aMatrix(1,4),aMatrix(2,1),aMatrix(2,2),aMatrix(2,3),aMatrix(2,4),aMatrix(3,1),aMatrix(3,2),aMatrix(3,3),aMatrix(3,4), aMatrix(4,1), aMatrix(4,2),aMatrix(4,3),1.0f };
	PxMat44 matrix = PxMat44(transform);
	return PxTransform(matrix);
}

M44f TransformToMat(PxTransform& aTransform)
{
	PxMat44 mat = PxMat44(aTransform);
	M44f returnValue = {	mat.column0.x, mat.column1.x, mat.column2.x, mat.column3.x,
							mat.column0.y, mat.column1.y, mat.column2.y, mat.column3.y,
							mat.column0.z, mat.column1.z, mat.column2.z, mat.column3.z,
							mat.column0.w, mat.column1.w, mat.column2.w, mat.column3.w };
	return returnValue;
	return M44f::Transpose(returnValue);
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

GBPhysXActor* GBPhysX::GBCreateDynamicSphere(M44f aMatrixTransform, int aRadius, float aDensity)
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
	M44f transform = M44f();
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
	const float StepSize = 0.016f;

	myDeltaTime += aDeltaTime;
	while (myDeltaTime >= StepSize)
	{
		myDeltaTime -= StepSize;
		stepPhysics(StepSize);
	}
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

void GBPhysX::GBSetKinematicActorTargetPos(physx::PxRigidActor* aActor, V3F aPosition, M44f aRotation)
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

GBPhysXActor* GBPhysX::CreateStaticTriangleMeshObject(std::string& aFilePath, M44f aTransform)
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

M44f GBPhysXActor::GetTransformMatrix()
{
	auto mat = PxMat44(myRigidActor->getGlobalPose());
	return {	mat.column0.x, mat.column0.y, mat.column0.z, mat.column0.w, 
				mat.column1.x, mat.column1.y, mat.column1.z, mat.column1.w,
				mat.column2.x, mat.column2.y, mat.column2.z, mat.column2.w, 
				mat.column3.x, mat.column3.y, mat.column3.z, mat.column3.w };
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
		myGBPhysX->GBSetKinematicActorTargetPos(myRigidActor, aPos, M44f());
	}
	else
	{
		SYSERROR("Tried setting kinematic target pos of non kinematic rigid actor");
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

void GBPhysXCharacter::SetDeathMomentCharTransform(M44f& aTransform)
{
	myDeathMomentCharTransform = aTransform;
}

GBPhysXCharacter::GBPhysXCharacter()
{
	myIsCrouching = false;
	myIsGrounded = false;
	myFilterData = nullptr;
	myLinkToLookAt = { nullptr };
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

}

void GBPhysXCharacter::UpdateHitBoxes()
{
	if (myHitBoxes.size() > 0)
	{

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
