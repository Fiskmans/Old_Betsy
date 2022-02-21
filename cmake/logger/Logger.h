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

	template<class T>
	class ExecuteFunctionOnConstruct
	{
	public:
		ExecuteFunctionOnConstruct(T&& aFunctor) { aFunctor(); }
	};

}
#define SYSCRASH(text)				Logger::Log(Logger::Type::SystemCrash,text);
#define SYSERROR(error, ...)		Logger::Rapport(Logger::Type::SystemError,Logger_Local::DirtySubstring(__FILE__), error, {__VA_ARGS__});
#define SYSINFO(text)				Logger::Log(Logger::Type::SystemInfo,text);
#define SYSWARNING(warning, ...)	Logger::Rapport(Logger::Type::SystemWarning,Logger_Local::DirtySubstring(__FILE__), warning, {__VA_ARGS__});
#define SYSVERBOSE(text)			Logger::Log(Logger::Type::SystemVerbose,text);
#define SYSNETWORK(text)			Logger::Log(Logger::Type::SystemNetwork,text);

#define LOGINFO(text)				Logger::Log(Logger::Type::Info,text);
#define LOGWARNING(text)			Logger::Log(Logger::Type::Warning,text);
#define LOGERROR(text)				Logger::Log(Logger::Type::Error,text);
#define LOGVERBOSE(text)			Logger::Log(Logger::Type::Verbose,text);

#define ONETIMEWARNING(warning, ...) { static Logger_Local::ExecuteFunctionOnConstruct executeOnce([]() -> void { SYSWARNING(warning, __VA_ARGS__); }); }

typedef unsigned short LoggerType;

namespace Logger
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

	void Rapport(LoggerType aType, const std::string& aFile, const std::string& aError, const std::vector<std::string>& aArguments);
	void Log(LoggerType aType, const std::string& aMessage);
	void Log(LoggerType aType, const std::wstring& aMessage);
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