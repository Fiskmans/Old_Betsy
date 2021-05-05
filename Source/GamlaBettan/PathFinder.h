#pragma once
#include <Singleton.hpp>
#include <string>
#include <SlabRay.h>
#include <Grid.h>

struct PathFinderData;
struct Node;

struct NavmeshWall
{
	V3F start;
	V3F end;
	V3F top;
	CommonUtilities::Plane<float> plane;
};

class PathFinder : public CommonUtilities::Singleton<PathFinder>
{
public:
	PathFinder();
	~PathFinder();
	void GenerateFromMesh(const std::string& aFilePath);
	void DrawDebug();
	V3F FindPoint(SlabRay aRay);
	std::vector<V3F> FindPath(V3F aFrom, V3F aTo);
	V3F Floorify(V3F aPoint);

	void Imgui();
	PathFinderData* GetMyPathFinderData();

	bool IntersectionWithWalls(V3F aStart, V3F aEnd);

private:
	bool FindPath(Node* aStart, Node* aEnd, std::vector<Node*>& aOutPath);
	void OptimizePath(V3F aStart, V3F aEnd, const std::vector<Node*>& aNodesToPass, std::vector<V3F>& aOutPath);

	void OBJLoader(const std::string& aFilePath);
	void FBXLoader(const std::string& aFilePath);

	void FindNodeCentersAndPlanes();
	void LinkNodes();
	void Verify();
	void Gridify();

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

	PathFinderData* myNavMesh;
	Node* FindNode(SlabRay aRay);
	Grid<Node> myGrid;
	std::vector<NavmeshWall> myNavmeshWalls;
};

