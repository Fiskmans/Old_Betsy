#include <pch.h>
#include "Octree.h"
#include "SlabRay.h"
#include "Entity.h"
#include "Collision.h"
#include "IntersectionRecord.h"

Octree::Octree() :
	myChildren{ nullptr },
	myParent(nullptr)
{

}

Octree::Octree(CommonUtilities::AABB3D<float> aBoundingBox, int aIndexInParent) :
	myBoundingBox(aBoundingBox),
	myChildren{ nullptr },
	myCurrentLifeSpan(-1),
	activeChildNodes(0),
	myIndexInParent(aIndexInParent)
{
	myMovedObjects.reserve(100);
}

Octree::Octree(CommonUtilities::AABB3D<float> aBoundingBox, std::vector<Collision*> aVectorOfOctreeObjects, int aIndexInParent) :
	myBoundingBox(aBoundingBox),
	myChildren{ nullptr },
	myObjects(aVectorOfOctreeObjects),
	myCurrentLifeSpan(-1),
	activeChildNodes(0),
	myIndexInParent(aIndexInParent)
{

}

Octree::~Octree()
{
	for (auto& it : myChildren)
	{
		SAFE_DELETE(it);
	}
}

void Octree::Update(float aDeltaTime)
{
	UpdateTree();

	if (ourTreeBuilt && ourTreeReady)
	{
		activeChildNodes = 0;
		for (int childIndex = 0; childIndex < 8; childIndex++)
		{
			if (myChildren[childIndex] != nullptr)
			{
				activeChildNodes++;
				myChildren[childIndex]->Update(aDeltaTime);
			}
		}

		if (myObjects.size() == 0)
		{
			if (activeChildNodes == 0)
			{
				if (myCurrentLifeSpan == -1)
				{
					myCurrentLifeSpan = myMaxLifeSpan;
				}
				else if (myCurrentLifeSpan > 0)
				{
					myCurrentLifeSpan--;
					if (myCurrentLifeSpan == 0)
					{
						//DELETE THIS NODE INSTEAD OF ++'

						myCurrentLifeSpan++;
					}
				}
			}
		}
		else
		{
			if (myCurrentLifeSpan != -1)
			{
				if (myMaxLifeSpan <= 64)
				{
					myMaxLifeSpan *= 2;
					myCurrentLifeSpan--;
				}
			}
		}
	}

	int myObjectsSize = CAST(int, myObjects.size());
	//std::vector<Collision*>* movedObjects = new std::vector<Collision*>();
	Collision* currentObject;

	for (int objectIndex = myObjectsSize - 1; objectIndex >= 0; objectIndex--)
	{
		currentObject = myObjects[objectIndex];
		if (currentObject->GetIsParentAlive())
		{
			if (currentObject->GetIsMoving())
			{
				myMovedObjects.push_back(currentObject);
			}
		}
		else
		{
			myObjects.erase(myObjects.begin() + objectIndex);
		}

	}

	for (int index = 0; index < 8; index++)
	{
		if (myChildren[index] != nullptr)
		{
			if (myChildren[index]->myCurrentLifeSpan == 0)
			{
				if (myChildren[index]->myObjects.size() > 0)
				{
					myChildren[index]->myCurrentLifeSpan = -1;
				}
				else
				{
					//TODO delete myChildren[index]?
					myChildren[index] = nullptr;
					activeChildNodes -= 1;
				}
			}
		}
	}

	for (int index = 0; index < 8; index++)
	{
		if (myChildren[index] != nullptr)
		{
			myChildren[index]->Update(aDeltaTime);
		}
	}


	// FOREACH MOVED OBJECT

	Octree* currentNode = this;
	int movedObjectsSize = CAST(int, myMovedObjects.size());
	//Collision* currentObject;

	//check if contained in current box or should move up

	for (int index = 0; index < movedObjectsSize; index++)
	{
		currentObject = myMovedObjects[index];
		CommonUtilities::AABB3D<float> currentBoxCol = *(currentObject->GetBoxCollider());
		if (currentNode->myBoundingBox.IsInside(currentBoxCol.Min()) && currentNode->myBoundingBox.IsInside(currentBoxCol.Max()))
		{
			continue;
		}
		else
		{
			if (myParent != nullptr)
			{
				bool foundContainingNode = false;
				Octree* parent = this;
				while (!foundContainingNode)
				{
					if (parent->myParent != nullptr)
					{
						parent = parent->myParent;
						if (parent->myBoundingBox.IsInside(currentBoxCol.Min()) && parent->myBoundingBox.IsInside(currentBoxCol.Max()))
						{
							//FOUND CONTAINING BOX
							//parent = node that fully contains the object
							foundContainingNode = true;
							break;
						}
					}
					else
					{
						//WE ARE AT ROOT
						foundContainingNode = true;
						break;
					}
				}

				// Check if can push down from parent
				// INSERT THE OBJECT INTO parent or child of parent
				int index = 0;
				for (auto& it : myObjects)
				{
					if (it == currentObject)
					{
						myObjects.erase(myObjects.begin() + index);
						break;
					}
					index++;
				}
				parent->Insert(currentObject);

			}
			else
			{
				//ITEM IS OUTSIDE OF ROOT NODE DANGER DANGER!!!!!!
				//Or will it just stay in root? hmm?
				continue;
			}
		}
	}
	myMovedObjects.clear();
}

bool Octree::Insert(Collision* aObject)
{
	//TODO KOLLA OM JAG HAR FÖR MÅNGA OBJECT
	CommonUtilities::Vector3<float> curPos = (myBoundingBox.Min() + (myBoundingBox.Max() - myBoundingBox.Min()) / 2.0f);
	CommonUtilities::Vector3<float> minVector;
	CommonUtilities::Vector3<float> maxVector;

	if (activeChildNodes == 0)
	{
		if (myObjects.size() < myMaxNumberOfObjectsInNode)
		{
			myObjects.push_back(aObject);
			return true;
		}
		else
		{
			if (abs(myBoundingBox.Max().x - myBoundingBox.Min().x) > MIN_SIZE)
			{
				// Add object to me then push my objects down
				myObjects.push_back(aObject);

				int objectSize = CAST(int, myObjects.size());
				for (int objectIndex = (objectSize - 1); objectIndex >= 0; objectIndex--)
				{
					Collision* curObjectToMoveDown = myObjects[objectIndex];

					minVector = curObjectToMoveDown->GetBoxCollider()->Min() - curPos;
					maxVector = curObjectToMoveDown->GetBoxCollider()->Max() - curPos;

					if (minVector.x < 0.0f) { minVector.x = 0; }
					else { minVector.x = 1; }
					if (minVector.y < 0.0f) { minVector.y = 0; }
					else { minVector.y = 1; }
					if (minVector.z < 0.0f) { minVector.z = 0; }
					else { minVector.z = 1; }
					if (maxVector.x < 0.0f) { maxVector.x = 0; }
					else { maxVector.x = 1; }
					if (maxVector.y < 0.0f) { maxVector.y = 0; }
					else { maxVector.y = 1; }
					if (maxVector.z < 0.0f) { maxVector.z = 0; }
					else { maxVector.z = 1; }

					if (minVector != maxVector)
					{
						continue;
					}
					else
					{
						//INSERT INTO NEW CONTAINING CHILD
						char child = CAST(char, 4 * CAST(char, minVector.x) + 2 * CAST(char, minVector.y) + 1 * CAST(char, minVector.z));
						if (myChildren[child] == nullptr)
						{
							float myHalfWidth = (myBoundingBox.Max().x - myBoundingBox.Min().x) / 2.0f;
							CommonUtilities::Vector3<float> min = { (myBoundingBox.Min().x + myHalfWidth * (bool)(child & 4)), myBoundingBox.Min().y + myHalfWidth * (bool)(child & 2), myBoundingBox.Min().z + myHalfWidth * (bool)(child & 1) };
							CommonUtilities::Vector3<float> max = { min.x + myHalfWidth, min.y + myHalfWidth, min.z + myHalfWidth };
							myChildren[child] = new Octree(CommonUtilities::AABB3D<float>(min, max), child);
							myChildren[child]->myParent = this;
						}
						myObjects.erase(myObjects.begin() + objectIndex);
						myChildren[child]->Insert(curObjectToMoveDown);
					}
				}
				return true;
			}
			else
			{
				myObjects.push_back(aObject);
				return true;
			}
		}
	}

	minVector = aObject->GetBoxCollider()->Min() - curPos;
	maxVector = aObject->GetBoxCollider()->Max() - curPos;

	if (minVector.x < 0.0f) { minVector.x = 0; }
	else { minVector.x = 1; }
	if (minVector.y < 0.0f) { minVector.y = 0; }
	else { minVector.y = 1; }
	if (minVector.z < 0.0f) { minVector.z = 0; }
	else { minVector.z = 1; }
	if (maxVector.x < 0.0f) { maxVector.x = 0; }
	else { maxVector.x = 1; }
	if (maxVector.y < 0.0f) { maxVector.y = 0; }
	else { maxVector.y = 1; }
	if (maxVector.z < 0.0f) { maxVector.z = 0; }
	else { maxVector.z = 1; }

	if (minVector != maxVector)
	{
		//STRADDLER THEREFOR PUT INSIDE MYSELF
		myObjects.push_back(aObject);
	}
	else
	{
		//CONTAINED IN CHILD
		//INSERT INTO NEW CONTAINING CHILD
		char child = CAST(char, 4 * CAST(char, minVector.x) + 2 * CAST(char, minVector.y) + 1 * CAST(char, minVector.z));
		if (myChildren[child] == nullptr)
		{
			float myHalfWidth = (myBoundingBox.Max().x - myBoundingBox.Min().x) / 2.0f;
			CommonUtilities::Vector3<float> min = { (myBoundingBox.Min().x + myHalfWidth * (bool)(child & 4)), myBoundingBox.Min().y + myHalfWidth * (bool)(child & 2), myBoundingBox.Min().z + myHalfWidth * (bool)(child & 1) };
			CommonUtilities::Vector3<float> max = { min.x + myHalfWidth, min.y + myHalfWidth, min.z + myHalfWidth };
			myChildren[child] = new Octree(CommonUtilities::AABB3D<float>(min, max));
			myChildren[child]->myParent = this;
		}
		myChildren[child]->Insert(aObject);
	}

	return true;
}

void Octree::UpdateTree()
{
	//if (!ourTreeBuilt)
	//{
	//	while (ourPendingInsertionList.GetSize() > 0)
	//	{
	//		myObjects.push_back(ourPendingInsertionList.Dequeue());
	//	}
	//	BuildTree();
	//}
	//else
	//{
	while (ourPendingInsertionList.size() > 0)
	{
		Insert(ourPendingInsertionList.front());
		ourPendingInsertionList.pop();
	}
	//}
	ourTreeBuilt = true;
	ourTreeReady = true;
}

void Octree::BuildTree()
{
	if (myObjects.size() <= 1)
	{
		ourTreeBuilt = true;
		return;
	}

	CommonUtilities::Vector3<float> dimensions = myBoundingBox.Max() - myBoundingBox.Min();

	if (dimensions == CommonUtilities::Vector3<float>(0.0f, 0.0f, 0.0f))
	{
		//FindEnclosingCube();
		dimensions = myBoundingBox.Max() - myBoundingBox.Min();
	}

	if (dimensions.x <= MIN_SIZE && dimensions.y <= MIN_SIZE && dimensions.z <= MIN_SIZE)
	{
		return;
	}

	CommonUtilities::Vector3<float> myHalfSize = dimensions / 2.0f;
	CommonUtilities::Vector3<float> myPos = myBoundingBox.Min() + myHalfSize;

	CommonUtilities::Vector3<float> min = myBoundingBox.Min();
	CommonUtilities::Vector3<float> max = myBoundingBox.Max();

	// 8 children
	std::array<CommonUtilities::AABB3D<float>, 8> octantList;

	octantList[0] = CommonUtilities::AABB3D<float>(min, myPos);
	octantList[1] = CommonUtilities::AABB3D<float>(CommonUtilities::Vector3<float>(min.x, min.y, myPos.z), CommonUtilities::Vector3<float>(myPos.x, myPos.y, max.z));
	octantList[2] = CommonUtilities::AABB3D<float>(CommonUtilities::Vector3<float>(min.x, myPos.y, min.z), CommonUtilities::Vector3<float>(myPos.x, max.y, myPos.z));
	octantList[3] = CommonUtilities::AABB3D<float>(CommonUtilities::Vector3<float>(min.x, myPos.y, myPos.z), CommonUtilities::Vector3<float>(myPos.x, max.y, max.z));

	octantList[4] = CommonUtilities::AABB3D<float>(CommonUtilities::Vector3<float>(myPos.x, min.y, min.z), CommonUtilities::Vector3<float>(max.x, myPos.y, myPos.z));
	octantList[5] = CommonUtilities::AABB3D<float>(CommonUtilities::Vector3<float>(myPos.x, min.y, myPos.z), CommonUtilities::Vector3<float>(max.x, myPos.y, max.z));
	octantList[6] = CommonUtilities::AABB3D<float>(CommonUtilities::Vector3<float>(myPos.x, myPos.y, min.z), CommonUtilities::Vector3<float>(max.x, max.y, myPos.z));
	octantList[7] = CommonUtilities::AABB3D<float>(CommonUtilities::Vector3<float>(myPos), CommonUtilities::Vector3<float>(max));

	//array of vectors of objects that fit in each octants bounding box
	std::array<std::vector<Collision*>, 8> octantArrayOfObjectVectors;

	//list of objects that could move down from this node
	std::vector<int> IdsOfObjectsThatGotMovedDown;

	for (int i = 0; i < myObjects.size(); i++)
	{
		for (int a = 0; a < 8; a++)
		{
			if (octantList[a].IsInside(myObjects[i]->GetBoxCollider()->Min()) && octantList[a].IsInside(myObjects[i]->GetBoxCollider()->Max()))
			{
				octantArrayOfObjectVectors[a].push_back(myObjects[i]);
				IdsOfObjectsThatGotMovedDown.push_back(i);
				break;
			}
		}
	}

	int vectorOffset = 0;
	//remove objects from current node if contained in child
	for (int id : IdsOfObjectsThatGotMovedDown)
	{
		myObjects.erase(myObjects.begin() + ((__int64)id - vectorOffset));
		vectorOffset++;
	}

	//create child nodes where there are items contained in the bounding box
	for (int a = 0; a < 8; a++)
	{
		if (octantArrayOfObjectVectors[a].size() > 0)
		{
			myChildren[a] = CreateNode(octantList[a], octantArrayOfObjectVectors[a], a);
			activeChildNodes += 1;
			myChildren[a]->BuildTree();
		}
	}

	ourTreeBuilt = true;
	ourTreeReady = true;
}

Octree* Octree::CreateNode(CommonUtilities::AABB3D<float> aBoundingBox, std::vector<Collision*> aObjectVector, int aIndexInParent)
{
	Octree* ret = new Octree(aBoundingBox, aObjectVector, aIndexInParent);
	ret->myParent = this;
	return ret;
}

std::vector<IntersectionRecord*> Octree::StartCollisionCheck(Collision* aCollider)
{
	if (aCollider->IsActive())
	{
		CheckCollison(myCollisionRecords, aCollider);
	}

	std::vector<IntersectionRecord*> returnValue = myCollisionRecords;
	myCollisionRecords.clear();
	return returnValue;
}

Entity* Octree::RayPickEntity(SlabRay& aRay)
{
	if (activeChildNodes > 0)
	{
		for (int i = 0; i < 8; i++)
		{
			if (myChildren[i])
			{
				if (CheckRayVSBB(myChildren[i]->myBoundingBox, aRay))
				{
					myChildren[i]->RayPickEntities(aRay, myHitEntitys);
				}
			}
		}
	}


	for (auto& it : myObjects)
	{
		if (it->IsEnabled())
		{
			CommonUtilities::AABB3D<float>* currentObjectBBox = it->GetBoxCollider();
			if (CheckRayVSBB(*currentObjectBBox, aRay))
			{
				myHitEntitys.push_back(it->GetParentEntityPtr());
			}
		}
	}


	Entity* nearestHitEntity = nullptr;
#pragma warning(suppress : 4056)
	float nearestDistance = _HUGE_ENUF;
	float distance;

	for (auto& it : myHitEntitys)
	{
		EntityType et = it->GetEntityType();
		if (/*et != EntityType::EnvironmentDestructible &&*/ et != EntityType::EnvironmentDynamic)
		{
			distance = (aRay.orig - it->GetPosition()).LengthSqr();
			if (distance < nearestDistance)
			{
				nearestHitEntity = it;
				nearestDistance = distance;
			}
		}
	}

	myHitEntitys.clear();
	return nearestHitEntity;
}

void Octree::RayPickEntities(SlabRay& aRay, std::vector<Entity*>& aHitEntitiesVector)
{
	if (activeChildNodes > 0)
	{
		for (int i = 0; i < 8; i++)
		{
			if (myChildren[i])
			{
				if (CheckRayVSBB(myChildren[i]->myBoundingBox, aRay))
				{
					myChildren[i]->RayPickEntities(aRay, aHitEntitiesVector);
				}
			}
		}
	}

	for (auto& it : myObjects)
	{
		if (it->IsEnabled())
		{
			CommonUtilities::AABB3D<float>* currentObjectBBox = it->GetBoxCollider();
			if (CheckRayVSBB(*currentObjectBBox, aRay))
			{
				aHitEntitiesVector.push_back(it->GetParentEntityPtr());
			}
		}
	}
}

bool Octree::CheckRayVSBB(const CommonUtilities::AABB3D<float>& aBB, const SlabRay& aRay, float* aNearestT)
{
	float tmin, tmax, tymin, tymax, tzmin, tzmax;

	V3F bounds[2];
	bounds[0] = aBB.Min();
	bounds[1] = aBB.Max();

	bool hit = true;

	tmin = (bounds[aRay.sign[0]].x - aRay.orig.x) * aRay.invdir.x;
	tmax = (bounds[1 - aRay.sign[0]].x - aRay.orig.x) * aRay.invdir.x;
	tymin = (bounds[aRay.sign[1]].y - aRay.orig.y) * aRay.invdir.y;
	tymax = (bounds[1 - aRay.sign[1]].y - aRay.orig.y) * aRay.invdir.y;

	if ((tmin > tymax) || (tymin > tmax))
	{
		return false;
	}
	if (tymin > tmin)
		tmin = tymin;
	if (tymax < tmax)
		tmax = tymax;

	tzmin = (bounds[aRay.sign[2]].z - aRay.orig.z) * aRay.invdir.z;
	tzmax = (bounds[1 - aRay.sign[2]].z - aRay.orig.z) * aRay.invdir.z;

	if ((tmin > tzmax) || (tzmin > tmax))
	{
		return false;
	}
	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;
	if (aNearestT)
	{
		*aNearestT = MIN(MIN(tmin, tymin), tzmin);
	}

	return true;
}



void Octree::CheckCollison(std::vector<IntersectionRecord*>& aIntersectionRecordVector, Collision* aCollider)
{
	CommonUtilities::AABB3D<float> boundingBox = *(aCollider->GetBoxCollider());
	//

	//TODO check active children to avoid 8 checks
	if (activeChildNodes > 0)
	{
		for (int i = 0; i < 8; i++)
		{
			if (myChildren[i])
			{
				//new
				CommonUtilities::AABB3D<float> childBB = myChildren[i]->myBoundingBox;
				V3F childPos = childBB.Min() + ((childBB.Max() - childBB.Min()) / 2.0f);

				childPos.x = CLAMP(boundingBox.Min().x, boundingBox.Max().x, childPos.x);
				childPos.y = CLAMP(boundingBox.Min().y, boundingBox.Max().y, childPos.y);
				childPos.z = CLAMP(boundingBox.Min().z, boundingBox.Max().z, childPos.z);

				if (myChildren[i]->myBoundingBox.IsInside(childPos))
					//end new

					//old
					//if (myChildren[i]->myBoundingBox.IsInside(boundingBox.Min()) || myChildren[i]->myBoundingBox.IsInside(boundingBox.Max()))
					//end old
				{
					myChildren[i]->CheckCollison(aIntersectionRecordVector, aCollider);
				}

			}
		}
	}

	for (auto& it : myObjects)
	{
		if (it->IsEnabled())
		{
			CommonUtilities::AABB3D<float>* currentObjectBBox = it->GetBoxCollider();
			//new
			V3F objectPos = currentObjectBBox->Min() + ((currentObjectBBox->Max() - currentObjectBBox->Min()) / 2.0f);

			objectPos.x = CLAMP(boundingBox.Min().x, boundingBox.Max().x, objectPos.x);
			objectPos.y = CLAMP(boundingBox.Min().y, boundingBox.Max().y, objectPos.y);
			objectPos.z = CLAMP(boundingBox.Min().z, boundingBox.Max().z, objectPos.z);

			if (currentObjectBBox->IsInside(objectPos))
				//end new

				//old
				//if (currentObjectBBox->IsInside(boundingBox.Min()) || currentObjectBBox->IsInside(boundingBox.Max()))
				//end old
			{
				CommonUtilities::Vector3<float> projectilePos = boundingBox.Min() + boundingBox.Max() - boundingBox.Min();
				CommonUtilities::Vector3<float> objectPos = currentObjectBBox->Min() + currentObjectBBox->Max() - currentObjectBBox->Min();
				CommonUtilities::Vector3<float> normal = (projectilePos - objectPos).GetNormalized();
				CommonUtilities::Vector3<float> bob = (projectilePos - objectPos);
				float distance = bob.Length();
				IntersectionRecord* collision = new IntersectionRecord(projectilePos, normal, aCollider, it, distance);
				aIntersectionRecordVector.push_back(collision);
			}
		}
	}
}

