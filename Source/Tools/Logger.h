#pragma once
#include <string>
#include <vector>

#ifndef STRING
#define STRING(arg) #arg
#endif
#ifndef STRINGVALUE
#define STRINGVALUE(arg) STRING(arg)
#endif
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

#define ONETIMEWARNING(warning,text) { static bool shoudShow = true; if(shoudShow) {shoudShow = false; SYSWARNING(warning,text); } }

const size_t COUNTERSTART = __COUNTER__;
#define INCREMENT (__COUNTER__ - COUNTERSTART - 1)
typedef unsigned short LoggerType;

#pragma warning(push)
#pragma warning(disable: 4369 4309)

namespace Logger
{
	enum Type : LoggerType
	{
		///types
		//system
		SystemInfo		= 1LL << INCREMENT,
		SystemError		= 1LL << INCREMENT,
		SystemCrash		= 1LL << INCREMENT,
		SystemWarning	= 1LL << INCREMENT,
		SystemVerbose	= 1LL << INCREMENT,
		SystemNetwork	= 1LL << INCREMENT,

		//game
		Info			= 1LL << INCREMENT,
		Warning			= 1LL << INCREMENT,
		Error			= 1LL << INCREMENT,
		Verbose			= 1LL << INCREMENT,

		///filters
		//global
		None			= LoggerType(0),
		All				= ~LoggerType(0),

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
#ifndef __INTELLISENSE__
static_assert(INCREMENT < sizeof(LoggerType) * CHAR_BIT - 1, "Ran out of bits for logger types");
#endif // !__INTELLISENSE__
#pragma warning(pop)