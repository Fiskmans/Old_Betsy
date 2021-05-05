#include "pch.h"
#include "PathFinder.h"
#include "PathFinderData.h"
#include "DebugDrawer.h"
#include <set>
#include <unordered_set>
#include <queue>

#pragma warning(push)
#pragma warning(disable: 26812)
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#pragma warning(pop)

#include <fstream>
#include <sstream>
#include <functional>
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>

#include "Intersection.hpp"

#if USEIMGUI
#include <imgui.h>
#include "WindowControl.h"
#endif

PathFinder::PathFinder() :
	myIsDisabled(false),
	myDrawBoundries(false),
	myDrawEdges(false),
	myDrawFlow(false),
	myDrawFunnelCalculations(false),
	myDrawLinks(false),
	myDrawNodes(false),
	myDrawPathfinding(false),
	myDrawPortals(false),
	myDrawVisited(false),
	myDrawWaypoints(false),
	myDrawWalls(false)
{
	myNavMesh = new PathFinderData();
}

PathFinder::~PathFinder()
{
	delete myNavMesh;
}

namespace Pathfinderhelpers
{
	struct Portal
	{
		V2F left;
		V2F right;
		float ly = 0.f;
		float ry = 0.f;
	};
	inline V3F OrthoNormalize(const V3F& aReference, const V3F& aOtherVector)
	{
		float dot = aOtherVector.Dot(aReference);
		auto projected = (aOtherVector - (dot * aReference));
		return projected.GetNormalized();
	}

	bool SameSide(const V3F& aTestPoint, const V3F& aReferencePoint, const V3F& aTrianglePointA, const V3F& aTrianglePointB)
	{
		V3F cpTest = (aTrianglePointB - aTrianglePointA).Cross((aTestPoint - aTrianglePointA));
		V3F cpRef = (aTrianglePointB - aTrianglePointA).Cross((aReferencePoint - aTrianglePointA));
		if (cpTest.Dot(cpRef) >= 0)
		{
			return true;
		}
		return false;
	}

	bool PointInTriangle(const V3F& aTestPoint, const V3F& aTrianglePointA, const V3F& aTrianglePointB, const V3F& aTrianglePointC)
	{
		if (SameSide(aTestPoint, aTrianglePointA, aTrianglePointB, aTrianglePointC) &&
			SameSide(aTestPoint, aTrianglePointB, aTrianglePointA, aTrianglePointC) &&
			SameSide(aTestPoint, aTrianglePointC, aTrianglePointA, aTrianglePointB))
		{
			return true;
		}
		return false;
	}

	bool AngleIsWithin(const V3F& aBasePoint, const V3F& aTestPoint, const V3F& aBoundsLeft, const V3F& aBoundsRight)
	{
		V3F toTarget = aTestPoint - aBasePoint;
		V3F toLeft = aBoundsLeft - aBasePoint;
		V3F toRight = aBoundsRight - aBasePoint;

		toTarget.y = 0;
		toLeft.y = 0;
		toRight.y = 0;

		toTarget.Normalize();
		toLeft.Normalize();
		toRight.Normalize();

		float leftAlignment = toTarget.Dot(toLeft);
		float rightAlignment = toTarget.Dot(toLeft);

		float span = toLeft.Dot(toRight);

		return leftAlignment > span && rightAlignment > span;
	}

	inline float triarea2(const V2F a, const V2F b, const V2F c)
	{
		const V2F ax = b - a;
		const V2F bx = c - a;
		return bx.x * ax.y - ax.x * bx.y;
	}

	inline bool vequal(const V2F a, const V2F b)
	{
		static const float eq = 0.001f * 0.001f;
		return (a - b).LengthSqr() < eq;
	}

	void AddIntersections(const std::vector<Portal>& aPortals, size_t aFrom, size_t aTo, V2F aStart, V2F aEnd, std::vector<V3F>& aOutPoints)
	{
		float discard;
		V3F start = V3F(aStart.x, 0.f, aStart.y);
		V3F end = V3F(aEnd.x, 0.f, aEnd.y);
		V3F to = end - start;
		V3F pNormal = to.Cross(V3F(0, 1, 0));
		CommonUtilities::Plane<float> plane(start, pNormal);

		V4F startColor = { 1.f, 0.f, 0.f, 1.f };
		V4F endColor = { 0.f, 0.f, 1.f, 0.2f };
		for (size_t i = aFrom; i < aTo; i++)
		{

			float part = (float(i - aFrom) / float(aTo - aFrom));

			V3F rayStart = V3F(aPortals[i].left.x, aPortals[i].ly, aPortals[i].left.y);
			V3F rayEnd = V3F(aPortals[i].right.x, aPortals[i].ry, aPortals[i].right.y);

			SlabRay ray(V4F(rayStart, 1.f), V4F(rayEnd, 1.f));
			V3F inters = ray.FindIntersection(plane, discard);
			if (inters == V3F(0.f, 0.f, 0.f))
			{
				inters = rayStart;
			}

			aOutPoints.push_back(inters);
		}

	}
	void stringPull(const std::vector<Portal>& aPortals, std::vector<V3F>& pts)
	{
		// Find straight path.
		// Init scan state
		V2F portalApex, portalLeft, portalRight;
		int apexIndex = 0, leftIndex = 0, rightIndex = 0;
		portalApex = aPortals[0].left;
		portalLeft = aPortals[0].left;
		portalRight = aPortals[0].right;

		// Add start point.

		pts.push_back(V3F(portalApex.x, aPortals[0].ly, portalApex.y));


		for (int i = 1; i < aPortals.size(); ++i)
		{
			V2F left = aPortals[i].left;
			V2F right = aPortals[i].right;
			float ly = aPortals[i].ly;
			float ry = aPortals[i].ry;

			// Update right vertex.
			if (triarea2(portalApex, portalRight, right) <= 0.0f)
			{
				if (vequal(portalApex, portalRight) || triarea2(portalApex, portalLeft, right) > 0.0f)
				{
					// Tighten the funnel.
					portalRight = right;
					rightIndex = i;
				}
				else
				{
					// Right over left, insert left to path and restart scan from portal left point.

					AddIntersections(aPortals, apexIndex, leftIndex, portalApex, portalLeft, pts);
					// Make current left the new apex.
					portalApex = portalLeft;
					apexIndex = leftIndex;
					// Reset portal
					portalLeft = portalApex;
					portalRight = portalApex;
					leftIndex = apexIndex;
					rightIndex = apexIndex;
					// Restart scan
					i = apexIndex;
					continue;
				}
			}

			// Update left vertex.
			if (triarea2(portalApex, portalLeft, left) >= 0.0f)
			{
				if (vequal(portalApex, portalLeft) || triarea2(portalApex, portalRight, left) < 0.0f)
				{
					// Tighten the funnel.
					portalLeft = left;
					leftIndex = i;
				}
				else
				{
					// Left over right, insert right to path and restart scan from portal right point.
					AddIntersections(aPortals, apexIndex, rightIndex, portalApex, portalRight, pts);
					// Make current right the new apex.
					portalApex = portalRight;
					apexIndex = rightIndex;
					// Reset portal
					portalLeft = portalApex;
					portalRight = portalApex;
					leftIndex = apexIndex;
					rightIndex = apexIndex;
					// Restart scan
					i = apexIndex;
					continue;
				}
			}
		}
	}
}

void PathFinder::GenerateFromMesh(const std::string& aFilePath)
{
	PERFORMANCETAG("Generating navmesh");
	static std::unordered_map < std::string, std::function<void(const std::string&)>> fileTypeMapping;
	if (fileTypeMapping.empty())
	{
		fileTypeMapping[".fbx"] = std::bind(&PathFinder::FBXLoader, this, std::placeholders::_1);
		fileTypeMapping[".obj"] = std::bind(&PathFinder::OBJLoader, this, std::placeholders::_1);
	}
	std::string extension = std::experimental::filesystem::path(aFilePath).extension().string();
	if (fileTypeMapping.count(extension) != 0)
	{
		PERFORMANCETAG("Loading");
		fileTypeMapping[extension](aFilePath);
	}
	else
	{
		SYSERROR("Unrecogniesed NavMesh File Format", aFilePath);
	}
	FindNodeCentersAndPlanes();
	Verify();
	LinkNodes();
	//Gridify();

	return;
}

void PathFinder::DrawDebug()
{
	if (myDrawNodes)
	{
		DebugDrawer::GetInstance().SetColor(myNodeColor);
		for (auto& node : myNavMesh->myNodes)
		{
			DebugDrawer::GetInstance().DrawLine(myNavMesh->myVertexCollection[node.myCorners[0]], myNavMesh->myVertexCollection[node.myCorners[1]]);
			DebugDrawer::GetInstance().DrawLine(myNavMesh->myVertexCollection[node.myCorners[1]], myNavMesh->myVertexCollection[node.myCorners[2]]);
			DebugDrawer::GetInstance().DrawLine(myNavMesh->myVertexCollection[node.myCorners[2]], myNavMesh->myVertexCollection[node.myCorners[0]]);
		}
	}
	if (myDrawLinks)
	{
		DebugDrawer::GetInstance().SetColor(myLinkColor);
		for (auto& node : myNavMesh->myNodes)
		{
			for (auto& link : node.myLinks)
			{
				DebugDrawer::GetInstance().DrawArrow(node.myCenter, link.toNode->myCenter);
			}
		}
	}
	if (myDrawWalls)
	{
		for (auto& wall : myNavmeshWalls)
		{
			DebugDrawer::GetInstance().DrawLine(wall.start, wall.end);
			DebugDrawer::GetInstance().DrawLine(wall.start, wall.top);
			DebugDrawer::GetInstance().DrawLine(wall.end, wall.end + V3F(0, 350, 0));
			DebugDrawer::GetInstance().DrawLine(wall.top, wall.end + V3F(0, 350, 0));
		}
	}
}

V3F PathFinder::FindPoint(SlabRay aRay)
{
#pragma warning(suppress : 4056)
	float closest = _HUGE_ENUF;
	if (myIsDisabled)
	{
		return aRay.FindIntersection(CommonUtilities::Plane<float>(V3F(0, 0, 0), V3F(0, 1, 0)), closest);
	}

	V3F result = V3F(0, 0, 0);

	for (auto& node : myNavMesh->myNodes)
	{
		//TODO: filter to increase performance
		float t;
		V3F inters = aRay.FindIntersection(node.myPlane, t);

		if (Pathfinderhelpers::PointInTriangle(inters,
			myNavMesh->myVertexCollection[node.myCorners[0]],
			myNavMesh->myVertexCollection[node.myCorners[1]],
			myNavMesh->myVertexCollection[node.myCorners[2]]))
		{
			if (t < closest)
			{
				closest = t;
				result = inters;
			}
		}
	}
	DebugDrawer::GetInstance().DrawCross(result, 5);
	return result;
}

std::vector<V3F> PathFinder::FindPath(V3F aFrom, V3F aTo)
{
	if (myIsDisabled)
	{
		return { aTo };
	}
	std::unordered_set<Node*> visited;
	std::vector<V3F> result;
	Node* start = FindNode(SlabRay(aFrom + V3F(0, 10000000.f, 0), V3F(0, -1, 0)));
	Node* end = FindNode(SlabRay(aTo + V3F(0, 10000000.f, 0), V3F(0, -1, 0)));
	std::vector<Node*> passedNodes;
	if (!start || !end || !FindPath(start, end, passedNodes))
	{
		return {};
	}
	OptimizePath(aFrom, aTo, passedNodes, result);
	result.push_back(aTo);
	return result;
}

V3F PathFinder::Floorify(V3F aPoint)
{
	return FindPoint(SlabRay(aPoint + V3F(0, 100, 0), V3F(0, -1, 0)));
}

void PathFinder::Imgui()
{
#if USEIMGUI
	WindowControl::Window("Pathfinder", [&]()
		{
			ImGui::Checkbox("Disable Pathfinder", &myIsDisabled);
			ImGui::Checkbox("Draw nodes", &myDrawNodes);
			ImGui::ColorEdit4("Node color", &myNodeColor.x);
			ImGui::Checkbox("Draw links", &myDrawLinks);
			ImGui::ColorEdit4("link color", &myLinkColor.x);
			ImGui::Checkbox("Draw searching", &myDrawPathfinding);
			ImGui::ColorEdit4("search color", &mySearchColor.x);
			if (myDrawPathfinding)
			{
				ImGui::Indent();
				ImGui::Checkbox("Draw visited", &myDrawVisited);
				ImGui::Checkbox("Draw flow", &myDrawFlow);
				ImGui::Checkbox("Draw passed edges", &myDrawEdges);
				ImGui::Checkbox("Draw Portals", &myDrawPortals);
				ImGui::Checkbox("Draw fitting bounds", &myDrawBoundries);
				ImGui::ColorEdit4("left color", &myLeftColor.x);
				ImGui::ColorEdit4("right color", &myRightColor.x);
				ImGui::Checkbox("Draw waypoints", &myDrawWaypoints);
				ImGui::Unindent();
			}
		});
#endif
}

PathFinderData* PathFinder::GetMyPathFinderData()
{
	return myNavMesh;
}

bool PathFinder::IntersectionWithWalls(V3F aStart, V3F aEnd)
{
	CommonUtilities::Ray<float> ray;

	V3F vec = aEnd - aStart;
	ray.InitWithOriginAndDirection(aStart, vec.GetNormalized());

	V3F intersectPoint;
	float shortestIntersectLength = 10000000;
	V3F shortestIntersctPoint;
	for (auto& wall : myNavmeshWalls)
	{
		if (CommonUtilities::IntersectionPlaneRay(wall.plane, ray, intersectPoint))
		{
			if (intersectPoint.x >= wall.start.x &&
				intersectPoint.x <= wall.end.x &&
				intersectPoint.z >= wall.start.z &&
				intersectPoint.z <= wall.end.z)
			{
				float intersectLength = V3F(intersectPoint - aStart).LengthSqr();

				if (intersectLength < shortestIntersectLength)
				{
					shortestIntersectLength = intersectLength;
					shortestIntersctPoint = intersectPoint;
				}
			}
		}
	}

	float targetLength = V3F(aEnd - aStart).LengthSqr();

	if (shortestIntersectLength < targetLength)
	{
		return true;
	}

	return false;
}

void PathFinder::OBJLoader(const std::string& aFilePath)
{
	myNavMesh->myNodes.clear();
	myNavMesh->myVertexCollection.clear();

	std::ifstream inFile;
	inFile.open(aFilePath);
	std::string buffer;
	while (std::getline(inFile, buffer))
	{
		std::stringstream ss(buffer);
		std::string type;
		ss >> type;
		if (type == "v")
		{
			float x, y, z;
			if (ss >> x >> y >> z)
			{
				myNavMesh->myVertexCollection.emplace_back(x, y, z);
			}
		}
		else if (type == "f")
		{
			Node nextNode;
#if TRACKLOADINGERROS
			for (size_t i = 0; i < 3; i++)
			{
				nextNode.myCorners[i] = 0ULL;
			}
#endif
			for (size_t i = 0; i < 3; i++)
			{
				std::string indexlist;
				if (ss >> indexlist)
				{
					std::stringstream indexStream(indexlist);
					indexStream >> nextNode.myCorners[i];
				}
			}
#if TRACKLOADINGERROS
			for (size_t i = 0; i < 3; i++)
			{
				if (nextNode.myCorners[i] == 0)
				{
					std::cout << "Error reading face" << std::endl;
				}
			}
#endif
			for (size_t i = 0; i < 3; i++)
			{
				nextNode.myCorners[i]--; // 0 indexed
			}
			myNavMesh->myNodes.push_back(nextNode);
		}
	}

	FindNodeCentersAndPlanes();
	LinkNodes();
}

void PathFinder::FBXLoader(const std::string& aFilePath)
{
	PERFORMANCETAG("FBX nav loader");
	SYSINFO("Generating nav mesh from: " + aFilePath);
	{
		PERFORMANCETAG("Exist check");
		if (!Tools::FileExists(aFilePath))
		{
			SYSERROR("Navmeshfile could not be found", aFilePath);
		}
	}
	const aiScene* scene;
	{
		PERFORMANCETAG("Importing");
		scene = aiImportFile(aFilePath.c_str(), aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded | aiProcess_Triangulate);
	}

	if (!scene)
	{
		SYSERROR("Could not load navmesh from", aFilePath);
		SYSERROR(aiGetErrorString(), aFilePath);
		return;
	}
	if (scene->mNumMeshes == 0)
	{
		SYSERROR("Nav mesh contains no meshes", aFilePath);
		aiReleaseImport(scene);
		return;
	}
	if (scene->mNumMeshes > 1)
	{
		SYSERROR("Nav mesh contains more than one mesh", aFilePath);
	}

	std::vector<std::set<size_t>> nodeIndexMapping;
	{
		PERFORMANCETAG("resize and reserve");
		nodeIndexMapping.resize(scene->mMeshes[0]->mNumVertices);
		myNavMesh->myNodes.clear();
		myNavMesh->myNodes.reserve(scene->mMeshes[0]->mNumFaces);
		myNavMesh->myVertexCollection.reserve(scene->mMeshes[0]->mNumVertices);
	}
	std::unordered_map<size_t, size_t> indexMapping;

	const auto ConvertToEngine = [](const aiVector3D& aVec) -> V3F
	{
		return V3F(aVec.x, aVec.y, aVec.z);
	};

	auto FindNeighbor = [&](const size_t& aIndex1, const size_t& aIndex2, size_t aNode) -> Node*
	{
		static std::vector<size_t> filtered;
		filtered.resize(scene->mMeshes[0]->mNumVertices);

		auto end = std::set_intersection(nodeIndexMapping[aIndex1].begin(), nodeIndexMapping[aIndex1].end(), nodeIndexMapping[aIndex2].begin(), nodeIndexMapping[aIndex2].end(), filtered.begin());
		auto it = filtered.begin();
		while (it != end)
		{
			if ((*it) != aNode)
			{
				return &myNavMesh->myNodes[*it];
			}
			it++;
		}
		return nullptr;
	};


	{
		PERFORMANCETAG("duplication checks");
		for (size_t indexIndex = 0; indexIndex < scene->mMeshes[0]->mNumVertices; indexIndex++)
		{
			V3F pos = ConvertToEngine(scene->mMeshes[0]->mVertices[indexIndex]);
			auto it = std::find(myNavMesh->myVertexCollection.begin(), myNavMesh->myVertexCollection.end(), pos);
			indexMapping[indexIndex] = it - myNavMesh->myVertexCollection.begin();
			if (it == myNavMesh->myVertexCollection.end())
			{
				myNavMesh->myVertexCollection.push_back(pos);
			}
		}
	}



	{
		PERFORMANCETAG("Face creation");
		for (size_t faceIndex = 0; faceIndex < scene->mMeshes[0]->mNumFaces; faceIndex++)
		{
			Node n;
			for (size_t i = 0; i < 3; i++)
			{
				n.myCorners[i] = indexMapping[scene->mMeshes[0]->mFaces[faceIndex].mIndices[i]];
			}
			myNavMesh->myNodes.push_back(n);
		}
	}


	{
		PERFORMANCETAG("Neighbor finding");
		for (size_t faceIndex = 0; faceIndex < scene->mMeshes[0]->mNumFaces; faceIndex++)
		{
			size_t at = scene->mMeshes[0]->mFaces[faceIndex].mIndices[0];
			for (size_t i = 1; i < scene->mMeshes[0]->mFaces[faceIndex].mNumIndices; i++)
			{
				Node* neighbor = FindNeighbor(at, scene->mMeshes[0]->mFaces[faceIndex].mIndices[i], faceIndex);
				if (neighbor)
				{
					Link l;
					l.toNode = neighbor;
					l.weight = (neighbor->myCenter - myNavMesh->myNodes[faceIndex].myCenter).Length();
					myNavMesh->myNodes[faceIndex].myLinks.push_back(l);
				}
				at = scene->mMeshes[0]->mFaces[faceIndex].mIndices[i];
			}
			Node* neighbor = FindNeighbor(at, scene->mMeshes[0]->mFaces[faceIndex].mIndices[0], faceIndex);
			if (neighbor)
			{
				Link l;
				l.toNode = neighbor;
				l.weight = (neighbor->myCenter - myNavMesh->myNodes[faceIndex].myCenter).Length();

				myNavMesh->myNodes[faceIndex].myLinks.push_back(l);
			}
		}
	}

	{
		PERFORMANCETAG("Releasing");
		aiReleaseImport(scene);
	}
}

void PathFinder::FindNodeCentersAndPlanes()
{
	PERFORMANCETAG("Node centers and planes");
	for (auto& i : myNavMesh->myNodes)
	{
		i.myCenter = V3F(0, 0, 0);
		i.myCenter += myNavMesh->myVertexCollection[i.myCorners[0]];
		i.myCenter += myNavMesh->myVertexCollection[i.myCorners[1]];
		i.myCenter += myNavMesh->myVertexCollection[i.myCorners[2]];
		i.myCenter = i.myCenter / 3.f;
		i.myPlane = CommonUtilities::Plane<float>(myNavMesh->myVertexCollection[i.myCorners[0]], myNavMesh->myVertexCollection[i.myCorners[1]], myNavMesh->myVertexCollection[i.myCorners[2]]);
	}
}

void PathFinder::LinkNodes()
{
	PERFORMANCETAG("Node linking");
	std::vector<std::set<Node*>> nodeMapping;
	std::vector<Node*> intersectionVector;
	intersectionVector.resize(2);

	const auto FindIntersection = [&](size_t aFirst, size_t aSecond, Node* aToIgnore) -> Node*
	{
		auto end = std::set_intersection(nodeMapping[aFirst].begin(), nodeMapping[aFirst].end(), nodeMapping[aSecond].begin(), nodeMapping[aSecond].end(), intersectionVector.begin());
		auto it = intersectionVector.begin();
		while (it != end)
		{
			if (*it != aToIgnore)
			{
				return *it;
			}
			it++;
		}
		return nullptr;
	};


	nodeMapping.resize(myNavMesh->myVertexCollection.size());
	for (auto& i : myNavMesh->myNodes)
	{
		nodeMapping[i.myCorners[0]].emplace(&i);
		nodeMapping[i.myCorners[1]].emplace(&i);
		nodeMapping[i.myCorners[2]].emplace(&i);
	}
	for (auto& node : myNavMesh->myNodes)
	{
		Node* neighbors[3];
		neighbors[0] = FindIntersection(node.myCorners[0], node.myCorners[1], &node);
		neighbors[1] = FindIntersection(node.myCorners[1], node.myCorners[2], &node);
		neighbors[2] = FindIntersection(node.myCorners[2], node.myCorners[0], &node);
		for (size_t i = 0; i < 3; i++)
		{
			if (neighbors[i])
			{
				Link l;
				l.toNode = neighbors[i];
				l.weight = (node.myCenter - neighbors[i]->myCenter).Length();
				l.myPoints[0] = node.myCorners[i];
				l.myPoints[1] = node.myCorners[(i + 1) % 3];
				node.myLinks.push_back(l);
			}
			else
			{
				NavmeshWall wall;
				wall.start = myNavMesh->myVertexCollection[node.myCorners[(i + 1) % 3]];
				wall.end = myNavMesh->myVertexCollection[node.myCorners[i]];
				wall.top = wall.start + V3F(0, 350, 0);
				wall.plane.InitWith3Points(wall.start, wall.end, wall.top);
				myNavmeshWalls.push_back(wall);
			}
		}
	}
}

void PathFinder::Verify()
{
	PERFORMANCETAG("Verify");
	for (auto& node : myNavMesh->myNodes)
	{
		if (node.myPlane.Normal().Length() < 0.5f)
		{
			SYSWARNING("Found dodgy nav node around " + node.myCenter.ToString(), "");
		}
	}
}

inline void FindMinMax(Node& node, V2F& aMin, V2F& aMax, PathFinderData* navmesh)
{
	aMin = V2F(CAST(float, INT_MAX), CAST(float, INT_MAX));
	aMax = V2F(CAST(float, INT_MIN), CAST(float, INT_MIN));

	for (auto& i : node.myCorners)
	{
		aMin.x = MIN(navmesh->myVertexCollection[i].x, aMin.x);
		aMin.y = MIN(navmesh->myVertexCollection[i].y, aMin.y);

		aMax.x = MAX(navmesh->myVertexCollection[i].x, aMax.x);
		aMax.y = MAX(navmesh->myVertexCollection[i].y, aMax.y);
	}
};

void PathFinder::Gridify()
{
	PERFORMANCETAG("Gridify");
	myGrid.Init({ -12800.f, -12800.f }, { 12800.f, 12800.f }, 100.f);
	V2F min;
	V2F max;
	CU::Vector2<int> gridMin;
	CU::Vector2<int> gridMax;

	for (auto& node : myNavMesh->myNodes)
	{
		FindMinMax(node, min, max, myNavMesh);

		gridMin = myGrid.GetGridIndex(min);
		gridMax = myGrid.GetGridIndex(max);

		for (int i = gridMin.x; i <= gridMax.x; i++)
		{
			for (int j = gridMin.y; j <= gridMax.y; j++)
			{
				myGrid(i, j).Add(&node, true);
			}
		}
	}
}

bool PathFinder::FindPath(Node* aStart, Node* aEnd, std::vector<Node*>& aOutPath)
{
	if (aStart == aEnd)
	{
		return true;
	}

	struct navNode
	{
		Node* node;
		float totalWeight;
		float hueristics;
	};

	struct navNodeComparer
	{
		bool operator()(const navNode& aLeft, const navNode& aRight)
		{
			return (aLeft.totalWeight + aLeft.hueristics) > (aRight.totalWeight + aRight.hueristics);
		}
	};

	std::unordered_map<Node*, Node*> backwardsMap;
	std::priority_queue<navNode, std::vector<navNode>, navNodeComparer> queue;
	std::unordered_set<Node*> visited;

	queue.push(navNode({ aEnd, 0, (aEnd->myCenter - aStart->myCenter).Length() }));
	while (!queue.empty())
	{
		navNode current = queue.top();
		queue.pop();
		for (auto& link : current.node->myLinks)
		{
			if (visited.find(link.toNode) == visited.end())
			{
				backwardsMap[link.toNode] = current.node;
				queue.push(navNode({ link.toNode,current.totalWeight + link.weight,(link.toNode->myCenter - aStart->myCenter).Length() }));
			}
		}
		visited.emplace(current.node);
		if (current.node == aStart)
		{
			break;
		}
	}

	if (myDrawPathfinding)
	{
		DebugDrawer::GetInstance().SetColor(mySearchColor);
		if (myDrawVisited)
		{
			for (auto& node : visited)
			{
				DebugDrawer::GetInstance().DrawLine(myNavMesh->myVertexCollection[node->myCorners[0]], myNavMesh->myVertexCollection[node->myCorners[1]]);
				DebugDrawer::GetInstance().DrawLine(myNavMesh->myVertexCollection[node->myCorners[1]], myNavMesh->myVertexCollection[node->myCorners[2]]);
				DebugDrawer::GetInstance().DrawLine(myNavMesh->myVertexCollection[node->myCorners[2]], myNavMesh->myVertexCollection[node->myCorners[0]]);
			}
		}
		if (myDrawFlow)
		{
			for (auto& link : backwardsMap)
			{
				DebugDrawer::GetInstance().DrawArrow(link.first->myCenter, link.second->myCenter);
			}
		}
	}

	if (visited.find(aStart) == visited.end())
	{
		return false;
	}


	Node* at = aStart;
	while (at != aEnd)
	{
		aOutPath.push_back(at);
		at = backwardsMap[at];
	}
	aOutPath.push_back(aEnd);
	return true;
}

void PathFinder::OptimizePath(V3F aStart, V3F aEnd, const std::vector<Node*>& aNodesToPass, std::vector<V3F>& aOutPath)
{
	if (aNodesToPass.empty())
	{
		return;
	}

	enum class ImortantPoint
	{
		First,
		Second,
		Both
	};

	struct passedEdge
	{
		Link* mylink;
		ImortantPoint myImportantPoint;
	};

	std::vector<passedEdge> collectedEdges;

	Node* at = aNodesToPass.front();
	for (size_t i = 1; i < aNodesToPass.size(); i++)
	{
		for (auto& link : at->myLinks)
		{
			if (link.toNode == aNodesToPass[i])
			{
				passedEdge edge;
				edge.mylink = &link;
				if (collectedEdges.empty())
				{
					edge.myImportantPoint = ImortantPoint::Both;
				}
				else
				{
					passedEdge last = collectedEdges.back();
					if (last.mylink->myPoints[0] == link.myPoints[0] || last.mylink->myPoints[1] == link.myPoints[0]) // share the first point of new link
					{
						edge.myImportantPoint = ImortantPoint::Second;
					}
					else
					{
						edge.myImportantPoint = ImortantPoint::First;
					}
				}

				collectedEdges.push_back(edge);
			}
		}
		at = aNodesToPass[i];
	}

	if (myDrawPathfinding)
	{
		DebugDrawer::GetInstance().SetColor(mySearchColor);
		if (myDrawEdges)
		{
			for (auto& edge : collectedEdges)
			{
				DebugDrawer::GetInstance().DrawLine(myNavMesh->myVertexCollection[edge.mylink->myPoints[0]], myNavMesh->myVertexCollection[edge.mylink->myPoints[1]]);
			}
		}
	}
	if (collectedEdges.empty())
	{
		return;
	}

	std::vector<Pathfinderhelpers::Portal> portals;

	V3F lastLeft = aStart;
	V3F lastRight = aStart;
	{
		Pathfinderhelpers::Portal portal;
		portal.left = V2F(lastLeft.x, lastLeft.z);
		portal.ly = lastLeft.y;
		portal.right = V2F(lastRight.x, lastRight.z);
		portal.ry = lastRight.y;
		portals.push_back(portal);
	}

	for (auto& edge : collectedEdges)
	{
		if (edge.myImportantPoint != ImortantPoint::Second)
		{
			lastLeft = myNavMesh->myVertexCollection[edge.mylink->myPoints[0]];
		}
		if (edge.myImportantPoint != ImortantPoint::First)
		{
			lastRight = myNavMesh->myVertexCollection[edge.mylink->myPoints[1]];
		}

		Pathfinderhelpers::Portal portal;
		portal.left = V2F(lastLeft.x, lastLeft.z);
		portal.ly = lastLeft.y;
		portal.right = V2F(lastRight.x, lastRight.z);
		portal.ry = lastRight.y;
		portals.push_back(portal);
	}

	lastRight = aEnd;
	lastLeft = aEnd;

	{
		Pathfinderhelpers::Portal portal;
		portal.left = V2F(lastLeft.x, lastLeft.z);
		portal.ly = lastLeft.y;
		portal.right = V2F(lastRight.x, lastRight.z);
		portal.ry = lastRight.y;
		portals.push_back(portal);
	}

	if (myDrawPortals)
	{
		DebugDrawer::GetInstance().SetColor(mySearchColor);
		for (size_t i = 0; i < portals.size(); i++)
		{
			V3F left = V3F(portals[i].left.x, portals[i].ly, portals[i].left.y);
			V3F right = V3F(portals[i].right.x, portals[i].ry, portals[i].right.y);
			DebugDrawer::GetInstance().DrawLine(left, right);
		}
	}
	if (myDrawPathfinding)
	{
		if (myDrawBoundries)
		{
			DebugDrawer::GetInstance().SetColor(myLeftColor);
			for (size_t i = 1; i < portals.size(); i++)
			{
				V3F last = V3F(portals[i - 1].left.x, portals[i - 1].ly, portals[i - 1].left.y);
				V3F next = V3F(portals[i].left.x, portals[i].ly, portals[i].left.y);
				DebugDrawer::GetInstance().DrawArrow(last, next);
			}
			DebugDrawer::GetInstance().SetColor(myRightColor);
			for (size_t i = 1; i < portals.size(); i++)
			{
				V3F last = V3F(portals[i - 1].right.x, portals[i - 1].ly, portals[i - 1].right.y);
				V3F next = V3F(portals[i].right.x, portals[i].ry, portals[i].right.y);
				DebugDrawer::GetInstance().DrawArrow(last, next);
			}
		}
	}
	Pathfinderhelpers::stringPull(portals, aOutPath);
	aOutPath.push_back(aEnd);

	/*
	aOutPath.push_back(aNodesToPass[i]->myCenter);
	*/
}

Node* PathFinder::FindNode(SlabRay aRay)
{
	Node* foundNode = nullptr;

#pragma warning(suppress : 4056)
	float closest = _HUGE_ENUF;

	for (auto& node : myNavMesh->myNodes)
	{
		//TODO: filter to increase performance
		float t;
		V3F inters = aRay.FindIntersection(node.myPlane, t);

		if (Pathfinderhelpers::PointInTriangle(inters,
			myNavMesh->myVertexCollection[node.myCorners[0]],
			myNavMesh->myVertexCollection[node.myCorners[1]],
			myNavMesh->myVertexCollection[node.myCorners[2]]))
		{
			if (t < closest)
			{
				closest = t;
				foundNode = &node;
			}
		}
	}
	return foundNode;
}
