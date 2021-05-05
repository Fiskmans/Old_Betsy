#include <pch.h>
#include "SvnIntegration.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#define SVNEXEPATH "C:/Program Files/TortoiseSVN/bin/svn.exe"
#define SVNEXEPATHCOMMAND "\"\"" SVNEXEPATH "\"\" "
#define SVNCOMMAND(arg) system(SVNEXEPATHCOMMAND arg)


inline bool exists(const std::string& name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

namespace SvnIntegration
{
	bool CheckForUpdates(char* aBuffer, int aSize)
	{
	if (SVNCOMMAND("status -u .. > svn_changes.log") == EXIT_SUCCESS)
	{
		std::ifstream inFile;
		inFile.open("svn_changes.log");
		std::string buffer;
		std::stringstream stream;
		bool newFile = false;
		while (inFile >> buffer)
		{
			if (buffer == "*")
			{
				std::getline(inFile, buffer);
				stream << buffer << std::endl;
				newFile = true;
			}
		}
		if (newFile)
		{
			if (stream.str().length() < aSize)
			{
				strcpy_s(aBuffer, aSize, stream.str().c_str());
			}
			else
			{
				memcpy(aBuffer, stream.str().c_str(), sizeof(char) * aSize);
				aBuffer[aSize - 1] = '\0';
			}
		}


		return newFile;
	}
	return false;
	}

	void Update()
	{
		SVNCOMMAND("update ..");
	}

	bool IsSupported()
	{
		return exists(SVNEXEPATH);
	}

	void LogMessages(char* aBuffer, int aSize)
	{
		SVNCOMMAND("log -l 20 .. > svn.log");


		std::ifstream infile;
		infile.open("svn.log");
		if (infile)
		{
			std::stringstream all;
			std::string row;

			int state = 0;
			int linecount = 0;
			while (std::getline(infile,row))
			{
				switch (state)
				{
				case 0: //nice header
					state++;
					break;
				case 1: //actual header
					{
						std::stringstream ss(row);
						std::string dummyData;
						std::string date;
						std::string time;
						std::string name;
						std::string rowCount;
						if (ss >> dummyData >> dummyData >> name >> dummyData >> date >> time >> dummyData >> dummyData >> dummyData >> dummyData >> dummyData >> dummyData >> linecount)
						{
							all << "[" << date << " "<< time << "] " << name << ":" << std::endl;
						}
					}
					state++;
					break;
				case 2: // empty line
					state++;
					break;
				case 3: //content
					all << row << std::endl;
					linecount--;
					if (linecount == 0)
					{
						all << std::endl;
						state = 0;
					}
				default:
					break;
				}

			}
			if (all.str().length() >= aSize)
			{
				memcpy(aBuffer, all.str().c_str(), aSize);
				aBuffer[aSize - 1] = '\0';
			}
			else
			{
				memcpy(aBuffer, all.str().c_str(), all.str().length()+1);
			}

		}
	}
}