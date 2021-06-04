#include "pch.h"
#include "PathFinder.h"

#include "CommonUtilities\Intersection.hpp"

#include "GamlaBettan\NavMeshLoader.h"

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
}

PathFinder::~PathFinder()
{
}

void PathFinder::SetNavMesh(const AssetHandle& aNavMesh)
{
	myNavMesh = aNavMesh;
}

namespace Pathfinderhelpers
{
	struct Portal
	{
		V2f left;
		V2f right;
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

	inline float triarea2(const V2f a, const V2f b, const V2f c)
	{
		const V2f ax = b - a;
		const V2f bx = c - a;
		return bx.x * ax.y - ax.x * bx.y;
	}

	inline bool vequal(const V2f a, const V2f b)
	{
		static const float eq = 0.001f * 0.001f;
		return (a - b).LengthSqr() < eq;
	}

	void AddIntersections(const std::vector<Portal>& aPortals, size_t aFrom, size_t aTo, V2f aStart, V2f aEnd, std::vector<V3F>& aOutPoints)
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

			FRay ray(V4F(rayStart, 1.f), V4F(rayEnd, 1.f));
			V3F inters;

			if (!ray.FindIntersection(plane,inters))
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
		V2f portalApex, portalLeft, portalRight;
		int apexIndex = 0, leftIndex = 0, rightIndex = 0;
		portalApex = aPortals[0].left;
		portalLeft = aPortals[0].left;
		portalRight = aPortals[0].right;

		// Add start point.

		pts.push_back(V3F(portalApex.x, aPortals[0].ly, portalApex.y));


		for (int i = 1; i < aPortals.size(); ++i)
		{
			V2f left = aPortals[i].left;
			V2f right = aPortals[i].right;
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

void PathFinder::DrawDebug()
{
	if (!myNavMesh.IsValid() || !myNavMesh.IsLoaded())
	{
		return;
	}
	NavMesh* mesh = myNavMesh.GetAsNavMesh();
	if (myDrawNodes)
	{
		DebugDrawer::GetInstance().SetColor(myNodeColor);
		for (auto& node : mesh->myNodes)
		{
			DebugDrawer::GetInstance().DrawLine(mesh->myVertexCollection[node.myCorners[0]], mesh->myVertexCollection[node.myCorners[1]]);
			DebugDrawer::GetInstance().DrawLine(mesh->myVertexCollection[node.myCorners[1]], mesh->myVertexCollection[node.myCorners[2]]);
			DebugDrawer::GetInstance().DrawLine(mesh->myVertexCollection[node.myCorners[2]], mesh->myVertexCollection[node.myCorners[0]]);
		}
	}
	if (myDrawLinks)
	{
		DebugDrawer::GetInstance().SetColor(myLinkColor);
		for (auto& node : mesh->myNodes)
		{
			for (auto& link : node.myLinks)
			{
				if (link.toNode != -1)
				{
					DebugDrawer::GetInstance().DrawArrow(node.myCenter, mesh->myNodes[link.toNode].myCenter);
				}
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

V3F PathFinder::FindPoint(const FRay& aRay)
{
	if (!myNavMesh.IsValid() || !myNavMesh.IsLoaded())
	{
		return V3F(0, 0, 0);
	}


	NavMesh* mesh = myNavMesh.GetAsNavMesh();
#pragma warning(suppress : 4056)
	float closest = _HUGE_ENUF;
	if (myIsDisabled)
	{
		aRay.FindIntersection(CommonUtilities::Plane<float>(V3F(0, 0, 0), V3F(0, 1, 0)), closest);
		return aRay.PointAtDistance(closest);
	}

	V3F result = V3F(0, 0, 0);

	for (auto& node : mesh->myNodes)
	{
		//TODO: filter to increase performance
		float t;
		V3F inters;
		if (!aRay.FindIntersection(node.myPlane, inters, t))
		{
			continue;
		}

		if (Pathfinderhelpers::PointInTriangle(inters,
			mesh->myVertexCollection[node.myCorners[0]],
			mesh->myVertexCollection[node.myCorners[1]],
			mesh->myVertexCollection[node.myCorners[2]]))
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
	if (myIsDisabled || !myNavMesh.IsValid() || !myNavMesh.IsLoaded())
	{
		return { aTo };
	}
	std::vector<V3F> result;
	NavMeshIndexType start = FindNode(FRay(aFrom + V3F(0, 10000000.f, 0), V3F(0, -1, 0)));
	NavMeshIndexType end = FindNode(FRay(aTo + V3F(0, 10000000.f, 0), V3F(0, -1, 0)));
	std::vector<NavMeshIndexType> passedNodes;
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
	return FindPoint(FRay(aPoint + V3F(0, 1000_m, 0), V3F(0, -1_m, 0)));
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


			if (ImGui::GetIO().KeyAlt)
			{
				bool open = true;
				ImGuiBackendFlags flags = ImGuiWindowFlags_None;
				flags |= ImGuiWindowFlags_AlwaysAutoResize; 
				flags |= ImGuiWindowFlags_NoScrollbar; 
				flags |= ImGuiWindowFlags_NoCollapse; 
				flags |= ImGuiWindowFlags_NoTitleBar;
				flags |= ImGuiWindowFlags_NoScrollWithMouse;

				ImGui::SetNextWindowPos(ImGui::GetIO().MousePos);

				if (ImGui::Begin("PF_ToolTip", &open, flags))
				{
					static const V4F colors[3] =
					{
						V4F(0.9f,0.6f,0.6f,1.0f),
						V4F(0.6f,0.9f,0.6f,1.0f),
						V4F(0.6f,0.6f,0.9f,1.0f)
					};

					ImGui::ColorButton("Color##0", ImVec4(colors[0].x, colors[0].y, colors[0].z, colors[0].w)); ImGui::SameLine();
					ImGui::ColorButton("Color##1", ImVec4(colors[1].x, colors[1].y, colors[1].z, colors[1].w)); ImGui::SameLine();
					ImGui::ColorButton("Color##2", ImVec4(colors[2].x, colors[2].y, colors[2].z, colors[2].w));

					NavMesh* mesh = myNavMesh.GetAsNavMesh();
					if (!mesh)
					{
						ImGui::Text("No navmesh");
					}
					else
					{
						NavMeshIndexType nodeindex = FindNode(*DebugTools::LastKnownMouseRay);

						ImGui::Text("Nodeindex: %d", nodeindex);

						if (nodeindex != -1)
						{
							NavMeshNode& node = mesh->myNodes[nodeindex];

							DebugDrawer::GetInstance().SetColor(colors[0]);
							DebugDrawer::GetInstance().DrawCross(mesh->myVertexCollection[node.myCorners[0]], 9);
							DebugDrawer::GetInstance().DrawLine(mesh->myVertexCollection[node.myCorners[0]], mesh->myVertexCollection[node.myCorners[1]]);
							if (node.myLinks[0].toNode != -1)
							{
								NavMeshNode& neighboor = mesh->myNodes[node.myLinks[0].toNode];
								DebugDrawer::GetInstance().DrawLine(LERP(mesh->myVertexCollection[neighboor.myCorners[0]], neighboor.myCenter, 0.1f), LERP(mesh->myVertexCollection[neighboor.myCorners[1]], neighboor.myCenter, 0.1f));
								DebugDrawer::GetInstance().DrawLine(LERP(mesh->myVertexCollection[neighboor.myCorners[1]], neighboor.myCenter, 0.1f), LERP(mesh->myVertexCollection[neighboor.myCorners[2]], neighboor.myCenter, 0.1f));
								DebugDrawer::GetInstance().DrawLine(LERP(mesh->myVertexCollection[neighboor.myCorners[2]], neighboor.myCenter, 0.1f), LERP(mesh->myVertexCollection[neighboor.myCorners[0]], neighboor.myCenter, 0.1f));
							}

							DebugDrawer::GetInstance().SetColor(colors[1]);
							DebugDrawer::GetInstance().DrawCross(mesh->myVertexCollection[node.myCorners[1]], 9);
							DebugDrawer::GetInstance().DrawLine(mesh->myVertexCollection[node.myCorners[1]], mesh->myVertexCollection[node.myCorners[2]]);
							if (node.myLinks[1].toNode != -1)
							{
								NavMeshNode& neighboor = mesh->myNodes[node.myLinks[1].toNode];
								DebugDrawer::GetInstance().DrawLine(LERP(mesh->myVertexCollection[neighboor.myCorners[0]], neighboor.myCenter, 0.1f), LERP(mesh->myVertexCollection[neighboor.myCorners[1]], neighboor.myCenter, 0.1f));
								DebugDrawer::GetInstance().DrawLine(LERP(mesh->myVertexCollection[neighboor.myCorners[1]], neighboor.myCenter, 0.1f), LERP(mesh->myVertexCollection[neighboor.myCorners[2]], neighboor.myCenter, 0.1f));
								DebugDrawer::GetInstance().DrawLine(LERP(mesh->myVertexCollection[neighboor.myCorners[2]], neighboor.myCenter, 0.1f), LERP(mesh->myVertexCollection[neighboor.myCorners[0]], neighboor.myCenter, 0.1f));
							}

							DebugDrawer::GetInstance().SetColor(colors[2]);
							DebugDrawer::GetInstance().DrawCross(mesh->myVertexCollection[node.myCorners[2]], 9);
							DebugDrawer::GetInstance().DrawLine(mesh->myVertexCollection[node.myCorners[2]], mesh->myVertexCollection[node.myCorners[0]]);
							if (node.myLinks[2].toNode != -1)
							{
								NavMeshNode& neighboor = mesh->myNodes[node.myLinks[2].toNode];
								DebugDrawer::GetInstance().DrawLine(LERP(mesh->myVertexCollection[neighboor.myCorners[0]], neighboor.myCenter, 0.1f), LERP(mesh->myVertexCollection[neighboor.myCorners[1]], neighboor.myCenter, 0.1f));
								DebugDrawer::GetInstance().DrawLine(LERP(mesh->myVertexCollection[neighboor.myCorners[1]], neighboor.myCenter, 0.1f), LERP(mesh->myVertexCollection[neighboor.myCorners[2]], neighboor.myCenter, 0.1f));
								DebugDrawer::GetInstance().DrawLine(LERP(mesh->myVertexCollection[neighboor.myCorners[2]], neighboor.myCenter, 0.1f), LERP(mesh->myVertexCollection[neighboor.myCorners[0]], neighboor.myCenter, 0.1f));
							}
						}
					}
				}
				ImGui::End();
			}


		});
#endif
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


inline void FindMinMax(NavMeshNode& node, V2f& aMin, V2f& aMax, NavMesh* navmesh)
{
	aMin = V2f(CAST(float, INT_MAX), CAST(float, INT_MAX));
	aMax = V2f(CAST(float, INT_MIN), CAST(float, INT_MIN));

	for (auto& i : node.myCorners)
	{
		aMin.x = MIN(navmesh->myVertexCollection[i].x, aMin.x);
		aMin.y = MIN(navmesh->myVertexCollection[i].y, aMin.y);

		aMax.x = MAX(navmesh->myVertexCollection[i].x, aMax.x);
		aMax.y = MAX(navmesh->myVertexCollection[i].y, aMax.y);
	}
};

bool PathFinder::FindPath(NavMeshIndexType aStart, NavMeshIndexType aEnd, std::vector<NavMeshIndexType>& aOutPath)
{
	if (aStart == aEnd)
	{
		return true;
	}

	if (!myNavMesh.IsValid() || !myNavMesh.IsLoaded() || aStart == -1 || aEnd == -1)
	{
		return false;
	}

	NavMesh* mesh = myNavMesh.GetAsNavMesh();

	struct navNode
	{
		NavMeshIndexType node;
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

	std::unordered_map<NavMeshIndexType, NavMeshIndexType> backwardsMap;
	std::priority_queue<navNode, std::vector<navNode>, navNodeComparer> queue;
	std::unordered_set<NavMeshIndexType> visited;

	queue.push(navNode({ aEnd, 0, (mesh->myNodes[aEnd].myCenter - mesh->myNodes[aStart].myCenter).Length() }));
	while (!queue.empty())
	{
		navNode current = queue.top();
		queue.pop();
		for (auto& link : mesh->myNodes[current.node].myLinks)
		{
			if (link.toNode == -1)
			{
				continue;
			}
			if (visited.find(link.toNode) == visited.end())
			{
				backwardsMap[link.toNode] = current.node;
				queue.push(navNode({ link.toNode,current.totalWeight + link.weight,(mesh->myNodes[link.toNode].myCenter - mesh->myNodes[aStart].myCenter).Length()
					}));
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
			for (auto& index : visited)
			{
				NavMeshNode& node = mesh->myNodes[index];
				DebugDrawer::GetInstance().DrawLine(mesh->myVertexCollection[node.myCorners[0]], mesh->myVertexCollection[node.myCorners[1]]);
				DebugDrawer::GetInstance().DrawLine(mesh->myVertexCollection[node.myCorners[1]], mesh->myVertexCollection[node.myCorners[2]]);
				DebugDrawer::GetInstance().DrawLine(mesh->myVertexCollection[node.myCorners[2]], mesh->myVertexCollection[node.myCorners[0]]);
			}
		}
		if (myDrawFlow)
		{
			for (auto& link : backwardsMap)
			{
				DebugDrawer::GetInstance().DrawArrow(mesh->myNodes[link.first].myCenter, mesh->myNodes[link.second].myCenter);
			}
		}
	}

	if (visited.find(aStart) == visited.end())
	{
		return false;
	}


	NavMeshIndexType at = aStart;
	while (at != aEnd)
	{
		aOutPath.push_back(at);
		at = backwardsMap[at];
	}
	aOutPath.push_back(aEnd);
	return true;
}

void PathFinder::OptimizePath(V3F aStart, V3F aEnd, const std::vector<NavMeshIndexType>& aNodesToPass, std::vector<V3F>& aOutPath)
{
	if (aNodesToPass.empty())
	{
		return;
	}

	NavMesh* mesh = myNavMesh.GetAsNavMesh();

	enum class ImortantPoint
	{
		First,
		Second,
		Both
	};

	struct passedEdge
	{
		NavMeshLink* mylink;
		NavMeshIndexType myLeft;
		NavMeshIndexType myRight;
		ImortantPoint myImportantPoint;
	};

	std::vector<passedEdge> collectedEdges;

	NavMeshIndexType at = aNodesToPass.front();
	for (size_t i = 1; i < aNodesToPass.size(); i++)
	{
		for (size_t l = 0; l < 3; l++)
		{
			if (mesh->myNodes[i].myLinks[l].toNode == aNodesToPass[i])
			{
				passedEdge edge;
				edge.mylink = &mesh->myNodes[i].myLinks[l];
				edge.myLeft = mesh->myNodes[i].myCorners[l];
				edge.myRight = mesh->myNodes[i].myCorners[(l+2)%3];

				if (collectedEdges.empty())
				{
					edge.myImportantPoint = ImortantPoint::Both;
				}
				else
				{
					passedEdge last = collectedEdges.back();
					if (last.myLeft == edge.myLeft || last.myRight == edge.myLeft) // share the first point of new link
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
				DebugDrawer::GetInstance().DrawLine(mesh->myVertexCollection[edge.myLeft], mesh->myVertexCollection[edge.myRight]);
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
		portal.left = V2f(lastLeft.x, lastLeft.z);
		portal.ly = lastLeft.y;
		portal.right = V2f(lastRight.x, lastRight.z);
		portal.ry = lastRight.y;
		portals.push_back(portal);
	}

	for (auto& edge : collectedEdges)
	{
		if (edge.myImportantPoint != ImortantPoint::Second)
		{
			lastLeft = mesh->myVertexCollection[edge.myLeft];
		}
		if (edge.myImportantPoint != ImortantPoint::First)
		{
			lastRight = mesh->myVertexCollection[edge.myRight];
		}

		Pathfinderhelpers::Portal portal;
		portal.left = V2f(lastLeft.x, lastLeft.z);
		portal.ly = lastLeft.y;
		portal.right = V2f(lastRight.x, lastRight.z);
		portal.ry = lastRight.y;
		portals.push_back(portal);
	}

	lastRight = aEnd;
	lastLeft = aEnd;

	{
		Pathfinderhelpers::Portal portal;
		portal.left = V2f(lastLeft.x, lastLeft.z);
		portal.ly = lastLeft.y;
		portal.right = V2f(lastRight.x, lastRight.z);
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
}

NavMeshIndexType PathFinder::FindNode(FRay aRay)
{
	NavMeshIndexType foundNode = -1;

	NavMesh* mesh = myNavMesh.GetAsNavMesh();

#pragma warning(suppress : 4056)
	float closest = _HUGE_ENUF;

	for(NavMeshIndexType i = 0; i < mesh->myNodes.size(); i++)
	{
		auto& node = mesh->myNodes[i];
		//TODO: filter to increase performance

		float t;
		V3F inters;
		aRay.FindIntersection(node.myPlane, inters, t);

		if (Pathfinderhelpers::PointInTriangle(inters,
			mesh->myVertexCollection[node.myCorners[0]],
			mesh->myVertexCollection[node.myCorners[1]],
			mesh->myVertexCollection[node.myCorners[2]]))
		{
			if (t < closest)
			{
				closest = t;
				foundNode = i;
			}
		}
	}
	return foundNode;
}
