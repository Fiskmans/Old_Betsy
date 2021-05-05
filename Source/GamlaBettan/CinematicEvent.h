#pragma once
class Scene;
class Camera;
class ParticleFactory;
class SpriteInstance;

namespace FiskJSON
{
	class Object;
}

class CinematicEvent
{
	friend class CinematicEditor;
public:
	virtual void Update(float aTime) = 0;
	virtual void AppendTo(FiskJSON::Object& aObject) = 0;
	virtual void Edit() = 0;
	
	void SetTiming(float aStart, float aDuration);
	void SetData(Scene* aScene, Camera* aCamera, ParticleFactory* aParticleFactory, SpriteInstance* aFadeSprite);
	void SetName(const std::string& aName);

	const char* GetName();
	virtual const char* GetType() = 0;

	void NameBox();

protected:
	void AppendCommon(FiskJSON::Object& aObject);

	bool IsActive(float aTime);
	float GetProgress(float aTime);
	Scene* myScene;
	Camera* myCamera;
	ParticleFactory* myParticleFactory;
	SpriteInstance* myFadeSprite;
	char myName[64] = { 0 };

	float GetDuration();

private:
	float myStart;
	float myDuration;
};




inline bool CinematicEvent::IsActive(float aTime)
{
	return aTime > myStart && (aTime-myStart) < myDuration;
}

inline float CinematicEvent::GetProgress(float aTime)
{
	return (aTime - myStart) / (myDuration);
}
inline float CinematicEvent::GetDuration()
{
	return myDuration;
}
inline void CinematicEvent::SetData(Scene* aScene, Camera* aCamera, ParticleFactory* aParticleFactory, SpriteInstance* aFadeSprite)
{
	myScene = aScene;
	myCamera = aCamera;
	myParticleFactory = aParticleFactory;
	myFadeSprite = aFadeSprite;
}

inline void CinematicEvent::SetName(const std::string& aName)
{
	strcpy_s(myName, 64, aName.c_str());
	if (aName.length() >= 64)
	{
		myName[63] = '\0';
	}
}

inline const char* CinematicEvent::GetName()
{
	return myName;
}