#pragma once

#include <map>
#include <string>
#include <vector>

class Camera;

struct ImGuiNodePackage
{
	std::map<std::string, std::vector<std::string>>* myFileList;
	Camera* myMainCamera;
};