#ifndef TOOLS_INTERSECTION_H
#define TOOLS_INTERSECTION_H

#include "tools/MathVector.h"
#include "tools/Plane.h"
#include "tools/PlaneVolume.h"
#include "tools/Sphere.h"
#include "tools/AABB3D.h"

namespace tools
{
	template<typename T>
	bool IntersectionPlaneRay(const tools::Plane<T>& aPlane, const tools::Ray<T>& aRay, tools::MathVector<T, 3>& aOutIntersectionPoint)
	{
		if (aPlane.Normal().Dot(aRay.Position() - aPlane.Point()) == 0)
		{
			return true;
		}
		if (aRay.Direction().Dot(aPlane.Normal()) == 0)
		{
			return false;
		}

		if (aPlane.Inside(aRay.Position()))
		{
			if (aPlane.Normal().Dot(aRay.Direction()) >= 0)
			{
				const T t = (aPlane.Point() - aRay.Position()).Dot(aPlane.Normal()) / aRay.Direction().Dot(aPlane.Normal());
				aOutIntersectionPoint = aRay.Position() + t * aRay.Direction();
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			if (aPlane.Normal().Dot(aRay.Direction()) >= 0)
			{
				return false;
			}
			else
			{
				const T t = (aPlane.Point() - aRay.Position()).Dot(aPlane.Normal()) / aRay.Direction().Dot(aPlane.Normal());
				aOutIntersectionPoint = aRay.Position() + t * aRay.Direction();
				return true;
			}
		}
	}

	template<typename T>
	bool IntersectionPlaneRay(const tools::Plane<T>& aPlane, const tools::Ray<T>& aRay, T& aOutIntersectionDistance)
	{
		if (aPlane.Normal().Dot(aRay.Position() - aPlane.Point()) == 0)
		{
			return true;
		}
		if (aRay.Direction().Dot(aPlane.Normal()) == 0)
		{
			return false;
		}

		if (aPlane.Inside(aRay.Position()))
		{
			if (aPlane.Normal().Dot(aRay.Direction()) >= 0)
			{
				const T t = (aPlane.Point() - aRay.Position()).Dot(aPlane.Normal()) / aRay.Direction().Dot(aPlane.Normal());
				aOutIntersectionDistance = t;
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			if (aPlane.Normal().Dot(aRay.Direction()) >= 0)
			{
				return false;
			}
			else
			{
				const T t = (aPlane.Point() - aRay.Position()).Dot(aPlane.Normal()) / aRay.Direction().Dot(aPlane.Normal());
				aOutIntersectionDistance = t;
				return true;
			}
		}
	}

	template<typename T>
	bool IntersectionAABBRay(const tools::AABB3D<T>& aAABB, const tools::Ray<T>& aRay, T& aOutDistance)
	{
		if (aAABB.IsInside(aRay.Position()))
		{
			aOutDistance = 0;
			return true;
		}
		tools::MathVector<T, 3> dirInverse = tools::MathVector<T, 3>({1,1,1}) / aRay.Direction();

		tools::MathVector<T, 3> minCorner = aAABB.Min();
		tools::MathVector<T, 3> maxCorner = aAABB.Max();

		tools::MathVector<T, 3> origin = aRay.Position();

		T t1 = (minCorner.x - origin.x) * dirInverse.x;
		T t2 = (maxCorner.x - origin.x) * dirInverse.x;

		T t3 = (minCorner.y - origin.y) * dirInverse.y;
		T t4 = (maxCorner.y - origin.y) * dirInverse.y;

		T t5 = (minCorner.z - origin.z) * dirInverse.z;
		T t6 = (maxCorner.z - origin.z) * dirInverse.z;

		T tmin = (std::max)((std::min)(t1, t2), (std::min)(t3, t4), (std::min)(t5, t6));
		T tmax = (std::min)((std::max)(t1, t2), (std::max)(t3, t4), (std::max)(t5, t6));

		aOutDistance = tmin;
		return !(tmax < 0) && !(tmin > tmax);
	}

	template<typename T>
	bool IntersectionAABBRay(const tools::AABB3D<T>& aAABB, const tools::Ray<T>& aRay)
	{
		T distance;
		return IntersectionAABBRay(aAABB, aRay, distance);
	}

	template<typename T>
	bool SolveQuadratic(const T aA, const T aB, const T aC, T& aX0, T& aX1)
	{
		const T discr = aB * aB - 4 * aA * aC;
		if (discr < 0)
		{
			return false;
		}
		else if (discr == 0)
		{
			aX0 = aX1 = -0.5 * aB / aA;
		}
		else
		{
			T q = (aB > 0) ?
				-0.5 * (double(aB) + std::sqrt(discr)) :
				-0.5 * (double(aB) - std::sqrt(discr));
			aX0 = q / aA;
			aX1 = aC / q;
		}

		if (aX0 > aX1)
		{
			std::swap(aX0, aX1);
		}

		return true;
	}

	template<typename T>
	bool IntersectionSphereRay(const tools::Sphere<T>& aSphere, const tools::Ray<T>& aRay, T& aResult)
	{
		if (aSphere.IsInside(aRay.Position())) 
		{
			aResult = 0;
			return true;
		}

		T t0;
		T t1;
		tools::MathVector<T, 3> L = aRay.Position() - aSphere.Position();
		const T a = aRay.Direction().Dot(aRay.Direction());
		const T b = 2 * aRay.Direction().Dot(L);
		const T c = L.Dot(L) - (aSphere.Radius() * aSphere.Radius());

		if (!SolveQuadratic<T>(a, b, c, t0, t1))
		{
			return false;
		}

		if (t0 > t1)
		{
			std::swap(t0, t1);
		}

		if (t0 < 0)
		{
			t0 = t1;
			if (t0 < 0)
			{
				return false;
			}
		}
		aResult = t0;
		return true;

	}

	template<typename T>
	bool IntersectionSphereRay(const tools::Sphere<T>& aSphere, const tools::Ray<T>& aRay, tools::MathVector<T, 3>& aIntersectionPoint)
	{
		T t = 0;
		bool result = IntersectionSphereRay(aSphere, aRay, t);

		if (result)
		{
			aIntersectionPoint = aRay.Position() + (aRay.Direction() * t);
		}

		return result;
	}

	template<typename T>
	bool IntersectionSphereRay(const tools::Sphere<T>& aSphere, const tools::Ray<T>& aRay)
	{
		T t;
		return IntersectionSphereRay(aSphere, aRay, t);
	}

	template<typename T>
	bool IntersectionSpherePlane(const tools::Sphere<T>& aSphere, const tools::Plane<T>& aPlane)
	{
		return aPlane.Inside(aSphere.Position() - aPlane.Normal() * aSphere.Radius());
	}

	template<typename T>
	bool IntersectionSphereSphere(const tools::Sphere<T>& aSphere1, const tools::Sphere<T>& aSphere2)
	{
		return (aSphere1.myPosition - aSphere2.myPosition).LengthSqr() < aSphere1.myRadius * aSphere1.myRadius + aSphere2.myRadius * aSphere1.myRadius;
	}

	template<typename T>
	inline bool IntersectionSpherePlaneVolume(const tools::Sphere<T>& aSphere, const tools::PlaneVolume<T>& aPlaneVolume)
	{
		for (auto& i : aPlaneVolume.myData)
		{
			if (!IntersectionSpherePlane(aSphere, i))
			{
				return false;
			}

#if VISUALIZESPHERETOPLANEVOLUMECHECK
			//DebugDrawer::GetInstance().DrawArrow(aSphere.Position(), aSphere.Position() - aSphere.Radius() * i.Normal());
#endif // VISUALIZESPHERETOPLANEVOLUMECHECK
		}

#if VISUALIZESPHERETOPLANEVOLUMECHECK
		DebugDrawer::GetInstance().DrawSphere(aSphere);
#endif // VISUALIZESPHERETOPLANEVOLUMECHECK

		return true;
	}

	template<typename T>
	inline bool IntersectionSphereFrustum(const tools::Sphere<T>& aSphere, const tools::PlaneVolume<T>& aPlaneVolume)
	{
		return IntersectionSpherePlane(aSphere, aPlaneVolume.Planes()[0]) && 
			IntersectionSpherePlane(aSphere, aPlaneVolume.Planes()[1]) && 
			IntersectionSpherePlane(aSphere, aPlaneVolume.Planes()[2]) && 
			IntersectionSpherePlane(aSphere, aPlaneVolume.Planes()[3]) && 
			IntersectionSpherePlane(aSphere, aPlaneVolume.Planes()[4]) && 
			IntersectionSpherePlane(aSphere, aPlaneVolume.Planes()[5]);
	}

	template<typename T>
	tools::MathVector<T, 3> SolvePlaneIntersection(const Plane<T>& a, const Plane<T>& b, const Plane<T>& c, size_t iterations = 40)
	{
		tools::MathVector<T, 3> point = { T(rand()),T(rand()) ,T(rand()) };
		for (size_t i = 0; i < iterations; i++)
		{
			point -= a.Normal() * ((point - a.Point()).Dot(a.Normal()));
			point -= b.Normal() * ((point - b.Point()).Dot(b.Normal()));
			point -= c.Normal() * ((point - c.Point()).Dot(c.Normal()));
		}
		return point;
	}

}
#endif