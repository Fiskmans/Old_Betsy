#ifndef TOOLS_VERSION_DUMPER_H
#define TOOLS_VERSION_DUMPER_H

#include "tools/Singleton.h"

#include <string>
#include <map>

#include <iostream>

namespace tools
{
	class SubsystemManager : public Singleton<SubsystemManager>
	{
	public:
		void AddSubsystemVersion(const std::string& aSubSystemName, const std::string& aVersionString);

		void LogAllVersions();

	private:
		std::map<std::string, std::string> myRegisteredSubsystems;
	};
}



#endif
