#ifndef DEMO_GAME1_ACTIONS_H
#define DEMO_GAME1_ACTIONS_H

#include "tools/Singleton.h"

#include "fisk_input/Input.h"

inline struct
{
	fisk::input::DigitalAction Jump;
	fisk::input::DigitalAction Dig;

	//fisk::input::AnalogueAction CameraHorizontal;
	//fisk::input::AnalogueAction CameraVertical;

} Actions;

#endif