#include "pch.h"
#include "WindSystem.h"

void WindSystem::Init(const V3F& aStartBaseWind)
{
	myGrid.Init({ -12800.f, -12800.f }, { 12800.f, 12800.f }, 25.f);

	for (size_t i = 0; i < myGrid.GetNumberOfNodes(); i++)
	{
		myGrid[i].Add(&aStartBaseWind);
	}

	AddWindToArea(V3F(3, 6, 8), V2F(0, 0), 50.f);
}

void WindSystem::Update(const float aDeltaTime)
{
	return; //we indoors, lol

    const int part = myGrid.GetNumberOfNodes() / 100;
	static int index = 0;

	for (int i = index; i <= index + part; i++)
	{
		myGrid[i].GetData()[0] = LERP(myGrid[i].GetData()[0], myBaseWind, aDeltaTime * 0.03f);
	}

	index += part;

	if (index >= myGrid.GetNumberOfNodes())
	{
		index = 0;
	}

}

void WindSystem::SetBaseWind(const V3F& aWindPower)
{
	myBaseWind = aWindPower;
}

void WindSystem::AddWindToArea(const V3F& aWindPower, const V3F& aPosition, const float aRadius)
{
	AddWindToArea(aWindPower, V2F(aPosition.x, aPosition.z), aRadius);
}

void WindSystem::AddWindToArea(const V3F& aWindPower, const V2F& aPosition, const float aRadius)
{
	CommonUtilities::Vector2<int> center = myGrid.GetGridIndex(aPosition);
	std::vector<CommonUtilities::Vector2<int>> circle;

	const int radius = MIN(myGrid.GetGridIndex(V2F(aRadius, 0)).x - myGrid.GetGridIndex(V2F(0, 0)).x, 50);
	int x = radius;
	int y = 0;
	int xChange = 1 - (x << 1);
	int yChagne = 0;
	int radiusError = 0;

	CommonUtilities::Vector2<int> firstToAdd;
	CommonUtilities::Vector2<int> secondToAdd;
	bool shouldAddFrist = true;
	bool shouldAddSecond = true;

	circle.reserve(MAX(50, CAST(int, radius * 192.5f - 2190.f))); //Approximation of how many nodes will be added (I used a graph calculator)

	while (x >= y)
	{
		for (int i = center.x - x; i <= center.x + x; i++)
		{
			shouldAddFrist = true;
			shouldAddSecond = true;

			firstToAdd = CommonUtilities::Vector2<int>(i, center.y + y);
			secondToAdd = CommonUtilities::Vector2<int>(i, center.y - y);

			if (firstToAdd == secondToAdd)
			{
				shouldAddSecond = false;
			}

			for (int i = circle.size() - 1; i >= 0; i--)
			{
				if (shouldAddFrist && circle[i] == firstToAdd)
				{
					shouldAddFrist = false;
				}

				if (shouldAddSecond && circle[i] == secondToAdd)
				{
					shouldAddSecond = false;
				}

				if (!shouldAddFrist && !shouldAddSecond)
				{
					break;
				}
			}

			if (shouldAddFrist)
			{
				circle.push_back(firstToAdd);
			}

			if (shouldAddSecond)
			{
				circle.push_back(secondToAdd);
			}
		}

		for (int i = center.x - y; i <= center.x + y; i++)
		{
			shouldAddFrist = true;
			shouldAddSecond = true;

			firstToAdd = CommonUtilities::Vector2<int>(i, center.y + x);
			secondToAdd = CommonUtilities::Vector2<int>(i, center.y - x);

			if (firstToAdd == secondToAdd)
			{
				shouldAddSecond = false;
			}

			for (int j = circle.size() - 1; j >= 0; j--)
			{
				if (shouldAddFrist && circle[j] == firstToAdd)
				{
					shouldAddFrist = false;
				}

				if (shouldAddSecond && circle[j] == secondToAdd)
				{
					shouldAddSecond = false;
				}

				if (!shouldAddFrist && !shouldAddSecond)
				{
					break;
				}
			}

			if (shouldAddFrist)
			{
				circle.push_back(firstToAdd);
			}

			if (shouldAddSecond)
			{
				circle.push_back(secondToAdd);
			}
		}

		y++;

		radiusError += yChagne;
		yChagne += 2;
		if ((radiusError << 1) + xChange > 0)
		{
			x--;
			radiusError += xChange;
			xChange += 2;
		}
	}

	const float r2 = 1.f / (radius * radius);

	for (auto& i : circle)
	{
		myGrid(i.x, i.y).GetData()[0] += aWindPower * (1.0f - (i.DistanceSqr(center) * r2));
	}

}

V3F WindSystem::GetWindAmount(const V3F& aPosition)
{
	return myGrid.GetNode(V2F(aPosition.x, aPosition.y)).GetData().front();
}
