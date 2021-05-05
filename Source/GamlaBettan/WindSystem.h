#pragma once
#include <Singleton.hpp>
#include <Vector3.hpp>
#include <Grid.h>

class WindSystem : public CommonUtilities::Singleton<WindSystem>
{
public:
	void Init(const V3F& aStartBaseWind);
	void Update(const float aDeltaTime);

	void SetBaseWind(const V3F& aWindPower);
	void AddWindToArea(const V3F& aWindPower, const V3F& aPosition, const float aRadius);
	void AddWindToArea(const V3F& aWindPower, const V2F& aPosition, const float aRadius);

	V3F GetWindAmount(const V3F& aPosition);

private:
	Grid<V3F> myGrid;
	V3F myBaseWind;
};

