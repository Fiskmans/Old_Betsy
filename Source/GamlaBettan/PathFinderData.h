#pragma once
#include <vector>
#include <Vector.hpp>
#include <Plane.hpp>
#include <PlaneVolume.hpp>


struct Node;

struct Link
{
	Node* toNode = nullptr;
	float weight = 0.f;
	size_t myPoints[2] = { 0, 0 };
};

struct Node
{
public:
	V3F myCenter;
	size_t myCorners[3] = { 0, 0, 0 };
	std::vector<Link> myLinks;
	CommonUtilities::Plane<float> myPlane;
};

struct PathFinderData
{
	std::vector<Node> myNodes;
	std::vector<V3F> myVertexCollection;
};