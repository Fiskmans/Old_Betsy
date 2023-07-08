
#include "tools/SubsystemManager.h"

#include "tools/Logger.h"

namespace tools {

	void SubsystemManager::AddSubsystemVersion(const std::string& aSubSystemName, const std::string& aVersionString)
	{
		myRegisteredSubsystems.emplace(aSubSystemName, aVersionString);
	}

	void SubsystemManager::LogAllVersions()
	{
		for (std::pair<std::string, std::string> systemAndVersion : myRegisteredSubsystems)
			fisk::tools::Log(fisk::tools::Type::SystemInfo, "using " + systemAndVersion.first + " version: " + systemAndVersion.second);
	}
}
