
#include "tools/SubsystemManager.h"

#include "logger/Logger.h"

namespace tools {

	void SubsystemManager::AddSubsystemVersion(const std::string& aSubSystemName, const std::string& aVersionString)
	{
		myRegisteredSubsystems.emplace(aSubSystemName, aVersionString);
	}

	void SubsystemManager::LogAllVersions()
	{
		for (std::pair<std::string, std::string> systemAndVersion : myRegisteredSubsystems)
			logger::Log(logger::Type::SystemInfo, "using " + systemAndVersion.first + " version: " + systemAndVersion.second);
	}
}
