#pragma once
class SpriteInstance;
class SpriteFactory;

class ProgressBar
{
public:
	ProgressBar() = default;
	~ProgressBar() = default;

	void Init(const V2F& aPos, const int* aValueToRepresent, const int* aMaxVauleToRepresent, const std::string& aBarPath, bool aIsHorizontal = true, const std::string& aBackgroundPath = "", const std::string& aForegroundPath = "", bool aGoFromUVMax = true);

	void Update();

	SpriteInstance* GetBackground();
	SpriteInstance* GetBar();
	SpriteInstance* GetForeground();

	static void SetSpriteFactory(SpriteFactory* aFactory);
	
private:
	SpriteInstance* myBackground;
	SpriteInstance* myBar;
	SpriteInstance* myForeground;

	V2F myPos;

	const int* myVaulePtr;
	const int* myMaxVaulePtr;

	bool myIsHorizontal;
	bool myFromUVMax;

	static SpriteFactory* ourSpriteFactory;

	void SetPivot(const V2F& aPivot);
};