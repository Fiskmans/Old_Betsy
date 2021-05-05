#pragma once
#include "CinematicEvent.h"
class FadeScreen : public CinematicEvent
{
public:
	FadeScreen(bool aOut);
	// Inherited via CinematicEvent
	virtual void Update(float aTime) override;
	virtual void AppendTo(FiskJSON::Object& aObject) override;
	virtual void Edit() override;
	virtual const char* GetType() override;

private:
	bool myOut;
};

inline FadeScreen::FadeScreen(bool aOut) : myOut(aOut)
{
}