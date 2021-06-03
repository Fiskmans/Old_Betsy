#pragma once

class SlabRay;
class Collision;
class Entity;
class IntersectionRecord;

class Octree
{
public:
	Octree();
	Octree(CommonUtilities::AABB3D<float> aBoundingBox, int aIndexInParent = -1);
	Octree(CommonUtilities::AABB3D<float> aBoundingBox, std::vector<Collision*> aVectorOfOctreeObjects, int aIndexInParent = -1);
	~Octree();

	CommonUtilities::AABB3D<float> myBoundingBox;

	void Update(float aDeltaTime);
	bool Insert(Collision* aObject);
	Octree* myChildren[8];
	char activeChildNodes = 0;
	int myMaxNumberOfObjectsInNode = 4;
	Octree* myParent;

	std::vector<Collision*> myObjects;
	
	const int MIN_SIZE = 8;
	int myMaxLifeSpan = 8;
	int myCurrentLifeSpan = -1;

	std::queue<Collision*> ourPendingInsertionList;
	bool ourTreeReady;
	bool ourTreeBuilt;
	
	
	void BuildTree();

	std::vector<IntersectionRecord*> StartCollisionCheck(Collision* aCollider);
	Entity* RayPickEntity(SlabRay& aRay);

	int myIndexInParent;

	static bool CheckRayVSBB(const CommonUtilities::AABB3D<float>& aBB, const SlabRay& aRay, float* aNearestT = nullptr);
private:
	void RayPickEntities(SlabRay& aRay, std::vector<Entity*>& aHitEntitiesVector);
	std::vector<Entity*> myHitEntitys;
	void CheckCollison(std::vector<IntersectionRecord*>& aIntersectionRecordVector, Collision* aCollider);
	std::vector<IntersectionRecord*> myCollisionRecords;
	std::vector<Collision*> myMovedObjects;
	void UpdateTree();

	Octree* CreateNode(CommonUtilities::AABB3D<float> aBoundingBox, std::vector<Collision*> aObjectVector, int aIndexInParent);
};
