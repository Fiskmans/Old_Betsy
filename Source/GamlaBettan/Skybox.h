#pragma once
#include "ModelInstance.h"

class Camera;
class Skybox : public ModelInstance
{

public:	
	Skybox(Model* aModel);
	void Render(Camera* aCamera);

private:

};
