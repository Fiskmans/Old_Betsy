#pragma once
#include <vector>
#include "Bezier.h"
#include <fstream>

template<class T,class FloatingPoint = float>
class Spline
{


public:
	Spline(std::string aName = "Spline");

	void Init(std::vector<T> aListOfPoints);
	void Init(const char* aFilePath);
	void Init(std::istream& aStream, size_t aCount);

	void MoveAlongSegment(FloatingPoint aT);
	void MoveAlongDistance(FloatingPoint aT,FloatingPoint aStepSize);
	bool Ended();

	T GetPosition();
	std::string& GetName();
	std::vector<T>& GetPoints();
private:
	void Setup();
	T GetNext();

	std::vector<T> myListOfPoints;
	std::array<T,4> myCurrentPoints;
	FloatingPoint myAT;
	size_t myCurrentIndex;
	std::string myName;
	bool myIsAtEnd = true;
};
template<class T, class FloatingPoint>
inline std::vector<T>& Spline<T, FloatingPoint>::GetPoints()
{
	return myListOfPoints;
}
template<class T, class FloatingPoint>
inline std::string& Spline<T, FloatingPoint>::GetName()
{
	return myName;
}

template<class T, class FloatingPoint>
inline Spline<T, FloatingPoint>::Spline(std::string aName)
{
	myName = aName;
}

template<class T, class FloatingPoint>
void Spline<T, FloatingPoint>::Init(std::vector<T> aListOfPoints)
{
	myListOfPoints = aListOfPoints;
	Setup();
}

template<class T, class FloatingPoint>
void Spline<T, FloatingPoint>::Init(const char* aFilePath)
{
	std::ifstream stream(aFilePath);
	T buffer;
	while (stream >> buffer)
	{
		myListOfPoints.push_back(buffer);
	}
	Setup();
}

template<class T, class FloatingPoint>
void Spline<T, FloatingPoint>::Init(std::istream& aStream, size_t aCount)
{
	T buffer;
	for (size_t i = 0; i < aCount; i++)
	{
		if (aStream >> buffer)
		{
			myListOfPoints.push_back(buffer);
		}
		else
		{
			break;
		}
	}
	Setup();
}

template<class T, class FloatingPoint>
void Spline<T, FloatingPoint>::MoveAlongSegment(FloatingPoint aT)
{
	myAT += aT;
	if (!myListOfPoints.empty() && myAT > 1.0)
	{
		myAT -= 1.0;
		myCurrentPoints[0] = myCurrentPoints[3];
		myCurrentPoints[1] = GetNext();
		myCurrentPoints[2] = GetNext();
		myCurrentPoints[3] = GetNext();
	}
}


template<class T, class FloatingPoint>
T Spline<T, FloatingPoint>::GetPosition()
{
	return Math::BezierInterpolation(myCurrentPoints, myAT);
}

template<class T, class FloatingPoint>
inline void Spline<T, FloatingPoint>::Setup()
{
	myIsAtEnd = false;
	myCurrentIndex = 0;
	myCurrentPoints[0] = GetNext();
	myCurrentPoints[1] = GetNext();
	myCurrentPoints[2] = GetNext();
	myCurrentPoints[3] = GetNext();
}

template<class T, class FloatingPoint>
inline T Spline<T, FloatingPoint>::GetNext()
{
	if (myCurrentIndex < myListOfPoints.size())
	{
		return myListOfPoints[myCurrentIndex++];
	}
	myIsAtEnd = true;
	return myListOfPoints.back();
}

template<class T, class FloatingPoint>
inline bool Spline<T, FloatingPoint>::Ended()
{
	return myIsAtEnd;
}
template<class T, class FloatingPoint>
inline void Spline<T, FloatingPoint>::MoveAlongDistance(FloatingPoint aDistance,FloatingPoint aStepsize)
{
	if (Ended())
	{
		return;
	}
	T lastpos = GetPosition();
	FloatingPoint counter = 0;
	size_t itc = 0;
	while (counter < aDistance)
	{
		MoveAlongSegment(aStepsize);
		T newpos = GetPosition();
		T delta = newpos - lastpos;
		counter += delta.Length();
		lastpos = newpos;
		++itc;
		if (itc > 250)
		{
			//SYSWARNING("Took more than 250 iterations to step through spline, giving up");
			break;
		}
	}
}