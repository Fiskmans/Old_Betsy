#pragma once
#include <BaseState.h>
#include <vector>
class ModelLoader;
class Scene;
class CinematicEvent;
class ParticleFactory;
class SpriteInstance;


class CinematicState : public BaseState
{
	friend class CinematicEditor;
	enum class State
	{
		Setup,
		Running,
		Cleanup
	};
public:
	bool Init(const std::string& aManuscript, Scene* aScene, ParticleFactory* aParticleFactory,SpriteFactory* aSpriteFactory);

	// Inherited via BaseState
	virtual void Update(const float aDeltaTime) override;
	virtual void Render(CGraphicsEngine* aGraphicsEngine) override;
	virtual void Activate() override;
	virtual void Deactivate() override;

private:

	void AddEvent(CinematicEvent* aEvent);
	bool Load(const std::string& aManuscript);

	std::vector<CinematicEvent*> myEvents;
	ModelLoader* myModelLoader = nullptr;
	ParticleFactory* myParticleFactory = nullptr;
	SpriteInstance* myFadeSprite = nullptr;

	State myState;
	V3F myCameraStartPos;
	V3F myCameraEndPos;
	Scene* myScene = nullptr;
	float myStartTime;
	float myTotalTime;

	float myCameaSpeed;
};

