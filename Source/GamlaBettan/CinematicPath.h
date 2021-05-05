#pragma once

namespace FiskJSON
{
	class Object;
}

class CinematicPath
{
public:
	CinematicPath();
	CinematicPath(V3F aEnd);
	void SetStart(V3F aStart);
	void SetControlPoints(V3F aC1, V3F aC2);
	V3F Evaluate(float aTime);
	void AppendTo(FiskJSON::Object* aObject);
	void ParseFrom(FiskJSON::Object& aObject);
	void Edit(V3F* aCurrent = nullptr);
	void AllowEditStart();

private:
	void VisualizePath();
	bool myCanEditStart;

	V3F myStart;
	V3F myEnd;

	bool myIsBezier;
	V3F myC1;
	V3F myC2;
};

