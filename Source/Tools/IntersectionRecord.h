#pragma once
#include "Collision.h"

class IntersectionRecord
{
public:
	IntersectionRecord(CommonUtilities::Vector3<float> aPos, CommonUtilities::Vector3<float> aNormal, Collision* aFirstCollider, Collision* aSecondCollider, float aDistance)
	{
		myPosition = aPos;
		myNormal = aNormal;
		myInterSectedObject1 = aFirstCollider;
		myInterSectedObject2 = aSecondCollider;
		myDistance = aDistance;
	}

	CommonUtilities::Vector3<float> myPosition;
	CommonUtilities::Vector3<float> myNormal;

	Collision* myInterSectedObject1;
	Collision* myInterSectedObject2;

	//Ray ray

	float myDistance;
private:
};

class IntersectionRecordBuilder
{
public:
	IntersectionRecordBuilder();
	~IntersectionRecordBuilder();

	CommonUtilities::Vector3<float> myPosition;
	CommonUtilities::Vector3<float> myNormal;
	Collision* myInterSectedObject1;
	Collision* myInterSectedObject2;
	float mydistance;

	IntersectionRecord* Build()
	{
		return new IntersectionRecord(myPosition, myNormal, myInterSectedObject1, myInterSectedObject2, mydistance);
	}
private:

};
