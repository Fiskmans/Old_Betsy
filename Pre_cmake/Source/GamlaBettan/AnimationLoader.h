#pragma once



typedef std::array<M44f, NUMBEROFANIMATIONBONES> AnimationFrame;
typedef std::vector<AnimationFrame> BakedAnimation;


class Animation
{
public:
	 void GetInterpolation(float aTime, float aWeight, AnimationFrame& aOutSnapshot);
	 bool GetInterpolationUnlooped(float aTime, float aWeight, AnimationFrame& aOutSnapshot);
	 float GetTimeToLoop(float aTime);

	float myWeights[NUMBEROFANIMATIONBONES];
	float myFPS;

private:
	friend class AnimationLoader;
	BakedAnimation myKeyFrames;
};

class AnimationLoader
{
private:
	friend class AssetManager;

	static Asset* BakeAnimation(const std::string& aPath);
};

