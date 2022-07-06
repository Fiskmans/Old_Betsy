#ifndef LOGGER_LOGGER_H
#define LOGGER_LOGGER_H

#include <string>
#include <vector>

#include "common/Macros.h"

namespace Logger_Local
{
	constexpr const char* DirtySubstring(const char* string)
	{
		const char* ret = string;
		while (*ret != '\0')
		{
			ret++;
		}
		while (*ret != '\\' && *ret != '/' && ret != string)
		{
			ret--;
		}
		ret++;
		return ret;
	}
}

#define LOG(type, message, ...)			logger::Rapport(type, Logger_Local::DirtySubstring(__FILE__), __LINE__, message, {__VA_ARGS__})

#define LOG_SYS_CRASH(message, ...)		LOG(logger::Type::SystemCrash, message, __VA_ARGS__)
#define LOG_SYS_ERROR(message, ...)		LOG(logger::Type::SystemError, message, __VA_ARGS__)
#define LOG_SYS_INFO(message, ...)		LOG(logger::Type::SystemInfo, message, __VA_ARGS__)
#define LOG_SYS_WARNING(message, ...)	LOG(logger::Type::SystemWarning, message, __VA_ARGS__)
#define LOG_SYS_VERBOSE(message, ...)	LOG(logger::Type::SystemVerbose, message, __VA_ARGS__)
#define LOG_SYS_NETWORK(message, ...)	LOG(logger::Type::SystemNetwork, message, __VA_ARGS__)

#define LOG_INFO(message, ...)			LOG(logger::Type::Info, message, __VA_ARGS__)
#define LOG_WARNING(message, ...)		LOG(logger::Type::Warning, message, __VA_ARGS__)
#define LOG_ERROR(message, ...)			LOG(logger::Type::Error, message, __VA_ARGS__)
#define LOG_VERBOSE(message, ...)		LOG(logger::Type::Verbose, message, __VA_ARGS__)

typedef unsigned short LoggerType;

namespace logger
{
	enum Type : LoggerType
	{
		///types
		//system
		SystemInfo		= 1LL << 0,
		SystemError		= 1LL << 1,
		SystemCrash		= 1LL << 2,
		SystemWarning	= 1LL << 3,
		SystemVerbose	= 1LL << 4,
		SystemNetwork	= 1LL << 5,

		//game
		Info			= 1LL << 6,
		Warning			= 1LL << 7,
		Error			= 1LL << 8,
		Verbose			= 1LL << 9,

		///filters
		//global
		None			= 0x0000,
		All				= 0xFFFF,

		//severity
		AnyInfo			= SystemInfo | Info,
		AnyWarning		= SystemWarning | Warning,
		AnyError		= SystemCrash | SystemError | Error,

		//type
		AnyGame			= Info | Warning | Error,
		AnySystem		= SystemCrash | SystemError | SystemInfo | SystemWarning,
		AnyVerbose		= Verbose | SystemVerbose | SystemNetwork,
		
		AllGame			= AnyGame | Verbose,
		AllSystem		= AnySystem | SystemVerbose
	};

	void Rapport(LoggerType aType, const std::string& aFile, const size_t aLine, const std::string& aError, const std::vector<std::string>& aArguments);
	void Log(LoggerType aType, const std::string& aMessage);
	void SetFilter(LoggerType aFilter);
	void SetHalting(LoggerType aFilter);
	void Map(LoggerType aMessageType, std::string aOutputFile);
	void UnMap(LoggerType aMessageType);
	void SetColor(LoggerType aMessageType, char aColor);
	void ImGuiLog();
	void RapportWindow();

	void SetupIcons(void* aHWND);

	void Shutdown();
}

#endif