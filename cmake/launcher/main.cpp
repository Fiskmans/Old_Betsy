#include <iostream>
#include <vector>
#include <cassert>

#include "versionConfig.h"
#include "tools/TimeHelper.h"

int main(int argc, char** argv)
{
	PERFORMANCETAG("main");

	std::cout << "Hello world from cmake" << std::endl;
	std::cout << "Launcher version: " << LAUNCHER_VERSION_STRING << std::endl;
	std::cout << "\tUsing Tools library version: " << TOOLS_VERSION_STRING << std::endl;
	std::cout << "\tUsing Imgui library version: " << IMGUI_VERSION_STRING << std::endl;
	std::cout << "\tUsing logger library version: " << LOGGER_VERSION_STRING << std::endl;
	int a;
	std::cin >> a;
}