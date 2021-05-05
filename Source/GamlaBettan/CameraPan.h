#pragma once
#include <Vector3.hpp>
#include "CinematicEvent.h"
#include "CinematicPath.h"

class CameraPan : public CinematicEvent
{
	friend class CinematicEditor;
	public:
		CameraPan(V3F aStart, V3F aEnd);
		CameraPan(CinematicPath aPath);
		void SetControlPoints(V3F aC1, V3F aC2);
		void SetPath(CinematicPath aPath);
		virtual void Update(float aTime) override;
		virtual void AppendTo(FiskJSON::Object& aObject) override;
		void VisualizePath();

	private:
		CinematicPath myPath;

		// Inherited via CinematicEvent
		virtual void Edit() override;

		// Inherited via CinematicEvent
		virtual const char* GetType() override;
};