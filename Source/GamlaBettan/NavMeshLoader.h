#pragma once
#include "Asset.h"

typedef unsigned int NavMeshIndexType;
const NavMeshIndexType NavMeshErrorIndex = static_cast<NavMeshIndexType>(-1);

struct NavmeshWall
{
	V3F start;
	V3F end;
	V3F top;
	CommonUtilities::Plane<float> plane;
};

struct NavMeshLink
{
	NavMeshIndexType toNode = NavMeshErrorIndex;
	float weight = 0.f;
};

struct NavMeshNode
{
public:
	V3F myCenter;
	NavMeshIndexType myCorners[3] = { 0, 0, 0 };
	NavMeshLink myLinks[3];
	CommonUtilities::Plane<float> myPlane;
};

struct NavMesh
{
	std::vector<NavMeshNode> myNodes;
	std::vector<V3F> myVertexCollection;
	std::vector<NavmeshWall> myWalls;
};

class NavMeshLoader
{
private:
	friend class AssetManager;
	static Asset* LoadNavMesh(const std::string& aPath);
};

