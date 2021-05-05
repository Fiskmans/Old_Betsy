#include <pch.h>
#include "BootLogger.h"

#include <windows.h>
#include <Lmcons.h>
#include <chrono>
#include <ctime>
#include <string>
#include <vector>

#include <sstream>
#include <fstream>
#ifdef _DEBUG
#include "../GamlaBettan/DebugTools.h"
#endif // _DEBUG


//TODO: Make safe
void CheckLaunchTimes(char* aTarget, int aBufferSize)
{
#ifdef _DEBUG
	char* start = aTarget;
	char username[UNLEN + 1];
	DWORD username_len = UNLEN + 1;
	GetUserNameA(username, &username_len);

	if (std::string(username) == "madeleine.lindaker")
	{
		strcpy_s(username , UNLEN + 1 ,"pyrola.lindaker");
	}

	DebugTools::myUsername = username;

	struct TimeStamp
	{
		std::string myName;
		long long myTime = 0;
	};

	std::vector<TimeStamp> timestamps;
	std::string row;
	std::ifstream inFile;
	inFile.open("LaunchTimes.txt");

	bool foundSelf = false;

	while (std::getline(inFile, row))
	{
		TimeStamp stamp;
		std::istringstream iss(row);
		if (iss >> stamp.myName >> stamp.myTime)
		{
			if (username == stamp.myName)
			{
				stamp.myTime = std::chrono::time_point_cast<std::chrono::hours>(std::chrono::system_clock::now()).time_since_epoch().count();
				foundSelf = true;
			}
			timestamps.push_back(stamp);
		}
	}
	inFile.close();

	if (!foundSelf)
	{
		timestamps.push_back({ username,std::chrono::time_point_cast<std::chrono::hours>(std::chrono::system_clock::now()).time_since_epoch().count() });
	}

	std::ofstream outFile;
	outFile.open("LaunchTimes.txt");

	for (auto& i : timestamps)
	{
		outFile << i.myName << " " << i.myTime << std::endl;
	}

	std::stringstream text;
	for (auto& i : timestamps)
	{
		long long hours = (std::chrono::time_point_cast<std::chrono::hours>(std::chrono::system_clock::now()).time_since_epoch().count() - i.myTime);

		text << i.myName << std::string(25- i.myName.length(),'.') <<  ((hours == 0) ? "Pretty much now" : ((hours == 1) ? "1 hour ago" : (std::to_string(hours) + " hours ago").c_str())) << std::endl;
	}
	text << '\0';
	if (strlen(text.str().c_str()) < aBufferSize)
	{
		strcpy_s(aTarget, aBufferSize, text.str().c_str());
	}


#endif // !_RETAIL
}