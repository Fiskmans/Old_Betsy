#pragma once

#include <map>
#include <string>
#include <vector>
//#include <Spline.h>
#include <Vector3.hpp>
class Camera;

struct ImGuiNodePackage
{
	std::map<std::string, std::vector<std::string>>* myFileList;
	Camera* myMainCamera;
};