#pragma once
#include "Vector3.hpp"
#include "Vector4.hpp"
#include "Plane.hpp"
#include "Ray.hpp"

class SlabRay
{
public:
	SlabRay(const V3F& orig, const V3F& dir) : orig(orig), dir(dir)
	{
		invdir.x = (dir.x != 0 ? 1.f / dir.x : 0.f);
		invdir.y = (dir.y != 0 ? 1.f / dir.y : 0.f);
		invdir.z = (dir.z != 0 ? 1.f / dir.z : 0.f);

		sign[0] = (invdir.x < 0);
		sign[1] = (invdir.y < 0);
		sign[2] = (invdir.z < 0);
	}

	SlabRay(const V4F& point1, const V4F& point2) : orig(point1), dir((point2-point1).GetNormalized())
	{
		invdir.x = (dir.x != 0 ? 1.f / dir.x : 0.f);
		invdir.y = (dir.y != 0 ? 1.f / dir.y : 0.f);
		invdir.z = (dir.z != 0 ? 1.f / dir.z : 0.f);

		sign[0] = (invdir.x < 0);
		sign[1] = (invdir.y < 0);
		sign[2] = (invdir.z < 0);
	}

	SlabRay operator=(const SlabRay& aRay)
	{
		orig = aRay.orig;
		dir = aRay.dir;
		invdir = aRay.invdir;
		sign[0] = aRay.sign[0];
		sign[1] = aRay.sign[1];
		sign[2] = aRay.sign[2];
		return *this;
	}

	V3F FindIntersection(CommonUtilities::Plane<float> aPlane,float& aT)
	{
		if (aPlane.Normal().Dot(orig - aPlane.Point()) == 0)
		{
			return orig;
		}
		if (dir.Dot(aPlane.Normal()) == 0)
		{
			return V3F(0,0,0);
		}

		if (aPlane.Inside(orig))
		{
			if (aPlane.Normal().Dot(dir) >= 0)
			{
				aT = (aPlane.Point() - orig).Dot(aPlane.Normal()) / dir.Dot(aPlane.Normal());
				return orig + aT * dir;
			}
			else
			{
				return V3F(0, 0, 0);
			}
		}
		else
		{
			if (aPlane.Normal().Dot(dir) >= 0)
			{
				return V3F(0, 0, 0);
			}
			else
			{
				aT = (aPlane.Point() - orig).Dot(aPlane.Normal()) / dir.Dot(aPlane.Normal());
				return orig + aT * dir;
			}
		}

		return V3F(0, 0, 0);
	}

	CommonUtilities::Ray<float> AsCU()
	{
		return CommonUtilities::Ray<float>(orig, orig + dir);
	}

	V3F orig, dir;
	V3F invdir;
	int sign[3];
};

