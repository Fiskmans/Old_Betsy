#pragma once
#include "CommonUtilities\Singleton.hpp"

#include "GamlaBettan\NavMeshLoader.h"

#include "Tools\SlabRay.h"
#include "Tools\Grid.h"


class PathFinder : public CommonUtilities::Singleton<PathFinder>
{
public:
	PathFinder();
	~PathFinder();
	void SetNavMesh(const AssetHandle& aNavMesh);
	void DrawDebug();
	V3F FindPoint(SlabRay aRay);
	std::vector<V3F> FindPath(V3F aFrom, V3F aTo);
	V3F Floorify(V3F aPoint);

	void Imgui();

	bool IntersectionWithWalls(V3F aStart, V3F aEnd);

private:
	NavMeshIndexType FindNode(SlabRay aRay);
	bool FindPath(NavMeshIndexType aStart, NavMeshIndexType aEnd, std::vector<NavMeshIndexType>& aOutPath);
	void OptimizePath(V3F aStart, V3F aEnd, const std::vector<NavMeshIndexType>& aNodesToPass, std::vector<V3F>& aOutPath);


	bool myIsDisabled;

	bool myDrawNodes;
	bool myDrawLinks;
	bool myDrawPathfinding;
	bool myDrawVisited;
	bool myDrawFlow;
	bool myDrawEdges;
	bool myDrawBoundries;
	bool myDrawPortals;
	bool myDrawWaypoints;
	bool myDrawFunnelCalculations;
	bool myDrawWalls;

	V4F myNodeColor = V4F(0.f, 0.f, 1.f, 1.f);
	V4F myLinkColor = V4F(0.7f, 0.f, 0.f, 1.f);
	V4F mySearchColor = V4F(0.2f, 1.f, 0.f, 1.f);
	V4F myLeftColor = V4F(0.2f, 1.f, 0.f, 1.f);
	V4F myRightColor = V4F(0.2f, 1.f, 0.f, 1.f);

	AssetHandle myNavMesh;
	Grid<NavMeshNode> myGrid;
	std::vector<NavmeshWall> myNavmeshWalls;
};

