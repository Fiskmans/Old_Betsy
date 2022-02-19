#pragma once
class InputManager;
class CGraphicsEngine;
class ForwardRenderer;
class WindowHandler;
class ModelLoader;
class SpriteFactory;
class LightLoader;

class BaseState
{
public:
	BaseState(bool shouldDeleteOnPop = true);
	virtual ~BaseState() = 0;
	virtual void Update() = 0;
	virtual void Render(CGraphicsEngine* aGraphicsEngine) = 0;

	inline bool IsMain() const;
	inline void SetMain(const bool aFlag);

	inline bool IsDrawThroughEnabled() const;
	inline void SetDrawThroughEnabled(const bool aFlag);

	inline bool IsUpdateThroughEnabled() const;
	inline void SetUpdateThroughEnabled(const bool aFlag);

	virtual void Activate();
	virtual void Deactivate();

	virtual void Unload();

	const bool myShouldDeleteOnPop;
protected:
	bool myIsMain;
	bool myDrawThrough;
	bool myUpdateThrough;
};

inline bool BaseState::IsDrawThroughEnabled() const
{
	return myDrawThrough;
}
inline void BaseState::SetDrawThroughEnabled(const bool aFlag)
{
	myDrawThrough = aFlag;
}

inline bool BaseState::IsUpdateThroughEnabled() const
{
	return myUpdateThrough;
}
inline void BaseState::SetUpdateThroughEnabled(const bool aFlag)
{
	myUpdateThrough = aFlag;
}
inline void BaseState::Activate()
{
}
inline void BaseState::Deactivate()
{
}
inline void BaseState::Unload()
{
}
inline bool BaseState::IsMain() const
{
	return myIsMain;
}
inline void BaseState::SetMain(const bool aFlag)
{
	myIsMain = aFlag;
}