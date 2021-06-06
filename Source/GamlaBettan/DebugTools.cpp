#include "pch.h"
#include "DebugTools.h"
#include "Scene.h"
#include "ModelInstance.h"

#include "AssetManager.h"
#include "CommonUtilities\Ray.hpp"
#include "CommonUtilities\Intersection.hpp"

#ifdef _DEBUG
FRay* DebugTools::LastKnownMouseRay = nullptr;
Camera* DebugTools::myCamera = nullptr;
bool DebugTools::myGizmoInScene = false;

ModelInstance* DebugTools::gizmoParts[6];
V3F* DebugTools::myGizmoVector;

void DebugTools::AttachToGizmo(V3F& aVector)
{
	myGizmoVector = &aVector;
}

bool DebugTools::IsGizmo(V3F& aVector)
{
	return &aVector == myGizmoVector;
}

void DebugTools::UpdateGizmo()
{
	if (myGizmoVector)
	{
		if (!myGizmoInScene)
		{
			for (size_t i = 0; i < 6; i++)
			{
				Scene::GetInstance().AddToScene(gizmoParts[i]);
			}
			myGizmoInScene = true;
		}
		for (size_t i = 0; i < 6; i++)
		{
			gizmoParts[i]->SetPosition(V4F(*myGizmoVector,1));
		}

		static std::bitset<3> axisControl;
		static const std::array<std::bitset<3>, 6> axisMapping =
		{
			1,//100
			2,//010
			4,//001

			3,//110
			5,//101
			6 //011
		};
		static CommonUtilities::Plane<float> movementPlane;
		const std::array<CommonUtilities::Plane<float>,6> planeMapping =
		{
			CommonUtilities::Plane<float>(*myGizmoVector,V3F(0,0,1)),
			CommonUtilities::Plane<float>(*myGizmoVector,V3F(1,0,0)),
			CommonUtilities::Plane<float>(*myGizmoVector,V3F(0,1,0)),

			CommonUtilities::Plane<float>(*myGizmoVector,V3F(0,0,1)),
			CommonUtilities::Plane<float>(*myGizmoVector,V3F(0,1,0)),
			CommonUtilities::Plane<float>(*myGizmoVector,V3F(1,0,0)),
		};

		const std::array<CommonUtilities::AABB3D<float>, 6> aabbs =
		{
			CommonUtilities::AABB3D<float>(*myGizmoVector + V3F(+30 ,-30 ,-30),*myGizmoVector + V3F(+180,+30 ,+30)),
			CommonUtilities::AABB3D<float>(*myGizmoVector + V3F(-30 ,+30 ,-30),*myGizmoVector + V3F(+30 ,+180,+30)),
			CommonUtilities::AABB3D<float>(*myGizmoVector + V3F(-30 ,-30 ,-180),*myGizmoVector + V3F(+30 ,+30 ,-30)),

			CommonUtilities::AABB3D<float>(*myGizmoVector + V3F(80,80,-20),*myGizmoVector + V3F(120,120,20)),
			CommonUtilities::AABB3D<float>(*myGizmoVector + V3F(80,-20,-120),*myGizmoVector + V3F(120,20,-80)),
			CommonUtilities::AABB3D<float>(*myGizmoVector + V3F(-20,80,-120),*myGizmoVector + V3F(20,120,-80)),
		};


		static V3F lastMousePos;
		static bool isMoving = false;

		char selected = -1;
		float dump;
		float closest = float(_HUGE_ENUF);

		for (size_t i = 0; i < 6; i++)
		{
			float t;
			if (CommonUtilities::IntersectionAABBRay(aabbs[i], *LastKnownMouseRay, t))
			{
				if (t < closest)
				{
					closest = t;
					selected = CAST(char, i);
				}
			}
		}

		for (size_t i = 0; i < 6; i++)
		{
			if (i==selected)
			{
				gizmoParts[i]->SetTint(V4F(0, 0, 0, 1));
			}
			else
			{
				gizmoParts[i]->SetTint(V4F(0, 0, 0, 0.5));
			}
		}

		if (GetAsyncKeyState(VK_LBUTTON))
		{
			if (isMoving)
			{
				V3F pos;
				LastKnownMouseRay->FindIntersection(movementPlane, pos);
				if (axisControl[0])
				{
					myGizmoVector->x += pos.x - lastMousePos.x;
				}
				if (axisControl[1])
				{
					myGizmoVector->y += pos.y - lastMousePos.y;
				}
				if (axisControl[2])
				{
					myGizmoVector->z += pos.z - lastMousePos.z;
				}
				lastMousePos = pos;
			}
			else
			{
				if (selected != -1)
				{
					isMoving = true;
					movementPlane = planeMapping[selected];
					axisControl = axisMapping[selected];

					LastKnownMouseRay->FindIntersection(movementPlane, lastMousePos, dump);
				}
			}
		}
		else
		{
			isMoving = false;
		}
	}
	else
	{
		if (myGizmoInScene)
		{
			for (size_t i = 0; i < 6; i++)
			{
				Scene::GetInstance().RemoveFromScene(gizmoParts[i]);
			}
			myGizmoInScene = false;
		}
	}
}

void DebugTools::Setup()
{
	gizmoParts[0] = AssetManager::GetInstance().GetModel("engine/x.fbx").InstansiateModel();
	gizmoParts[1] = AssetManager::GetInstance().GetModel("engine/y.fbx").InstansiateModel();
	gizmoParts[2] = AssetManager::GetInstance().GetModel("engine/z.fbx").InstansiateModel();

	gizmoParts[3] = AssetManager::GetInstance().GetModel("engine/xy.fbx").InstansiateModel();
	gizmoParts[4] = AssetManager::GetInstance().GetModel("engine/xz.fbx").InstansiateModel();
	gizmoParts[5] = AssetManager::GetInstance().GetModel("engine/yz.fbx").InstansiateModel();
}

#endif // DEBUG