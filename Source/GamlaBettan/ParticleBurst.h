#pragma once
#include "CinematicEvent.h"
#include <string>
#include <Vector3.hpp>
class ParticleInstance;
class Scene;

class ParticleBurst : public CinematicEvent
{
public:
	ParticleBurst(const std::string& aType, V3F aPosition, float aScale, V3F aDirection);

	// Inherited via CinematicEvent
	virtual void Update(float aTime) override;
	void AppendTo(FiskJSON::Object& aObject) override;
	void Edit() override;
	const char* GetType() override;
private:
	ParticleInstance* myInstance = nullptr;
	std::string myType;
	V3F myPosition;
	V3F myDirection;
	float myScale;
#if USEIMGUI
	char myNameBuffer[128] = { 0 };
#endif // USEIMGUI

};

inline ParticleBurst::ParticleBurst(const std::string& aType, V3F aPosition, float aScale, V3F aDirection) : myType(aType), myPosition(aPosition), myDirection(aDirection), myScale(aScale)
{
#if USEIMGUI
	size_t dotpos = aType.find('.');
	if (dotpos != std::string::npos)
	{
		memcpy(myNameBuffer, aType.c_str(), dotpos * sizeof(char));
	}
#endif // 

}

inline const char* ParticleBurst::GetType()
{
	return "particleBurst";
}