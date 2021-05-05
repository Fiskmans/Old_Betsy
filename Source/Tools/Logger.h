#pragma once
#include <string>
#include <vector>

#ifndef STRING
#define STRING(arg) #arg
#endif
#ifndef STRINGVALUE
#define STRINGVALUE(arg) STRING(arg)
#endif
namespace ThisNameSpaceIsHereToNeverCollideEver
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
#define SYSCRASH(text) Logger::Log(Logger::Type::SystemCrash,text);
#define SYSERROR(error, arg) Logger::Rapport(Logger::Type::SystemError,ThisNameSpaceIsHereToNeverCollideEver::DirtySubstring(__FILE__),error,arg); Logger::Log(Logger::Type::SystemError,std::string("[" + std::string(ThisNameSpaceIsHereToNeverCollideEver::DirtySubstring(__FILE__)) + ": " STRINGVALUE(__LINE__) "]") + error + arg);
#define SYSINFO(text) Logger::Log(Logger::Type::SystemInfo,text);
#define SYSWARNING(warning, arg) Logger::Rapport(Logger::Type::SystemWarning,ThisNameSpaceIsHereToNeverCollideEver::DirtySubstring(__FILE__),warning,arg); Logger::Log(Logger::Type::SystemWarning,warning + std::string(" ") + arg);
#define SYSVERBOSE(text) Logger::Log(Logger::Type::SystemVerbose,text);
#define SYSNETWORK(text) Logger::Log(Logger::Type::SystemNetwork,text);
#define LOGINFO(text) Logger::Log(Logger::Type::Info,text);
#define LOGWARNING(text) Logger::Log(Logger::Type::Warning,text);
#define LOGERROR(text) Logger::Log(Logger::Type::Error,text);
#define LOGVERBOSE(text) Logger::Log(Logger::Type::Verbose,text);
#define ONETIMEWARNING(warning,text) { static bool shoudShow = true; if(shoudShow) {shoudShow = false; SYSWARNING(warning,text); } }

const size_t COUNTERSTART = __COUNTER__;
#define INCREMENT (__COUNTER__ - COUNTERSTART - 1)
#define LOGGERTYPE unsigned short

#pragma warning(push)
#pragma warning(disable: 4369 4309)

namespace Logger
{
	enum Type : LOGGERTYPE
	{
		///types
		//system
		SystemInfo = 1LL << INCREMENT,
		SystemError = 1LL << INCREMENT,
		SystemCrash = 1LL << INCREMENT,
		SystemWarning = 1LL << INCREMENT,
		SystemVerbose = 1LL << INCREMENT,
		SystemNetwork = 1LL << INCREMENT,

		//game
		Info = 1LL << INCREMENT,
		Warning = 1LL << INCREMENT,
		Error = 1LL << INCREMENT,
		Verbose = 1LL << INCREMENT,

		///filters
		//global
		None = LOGGERTYPE(0),
		All = ~LOGGERTYPE(0),

		//severity
		AnyInfo = SystemInfo | Info,
		AnyWarning = SystemWarning | Warning,
		AnyError = SystemCrash | SystemError | Error,

		//type
		AnyGame = Info | Warning | Error,
		AnySystem = SystemCrash | SystemError | SystemInfo | SystemWarning,
		AnyVerbose = Verbose | SystemVerbose | SystemNetwork,

		AllGame = AnyGame | Verbose,
		AllSystem = AnySystem | SystemVerbose
	};
	void Rapport(LOGGERTYPE aType, const std::string& aCategory, const std::string& aError, const std::string& aArgument);
	void Log(LOGGERTYPE aType, const std::string& aMessage);
	void Log(LOGGERTYPE aType, const std::wstring& aMessage);
	void SetFilter(LOGGERTYPE aFilter);
	void SetHalting(LOGGERTYPE aFilter);
	void Map(LOGGERTYPE aMessageType, std::string aOutputFile);
	void UnMap(LOGGERTYPE aMessageType);
	void SetColor(LOGGERTYPE aMessageType, char aColor);
	void ImGuiLog();
	void RapportWindow();

	void SetupIcons(void* aHWND);

	void Shutdown();
}
#ifndef __INTELLISENSE__
static_assert(INCREMENT < sizeof(LOGGERTYPE) * CHAR_BIT - 1, "Ran out of bits for logger types");
#endif // !__INTELLISENSE__
#pragma warning(pop)