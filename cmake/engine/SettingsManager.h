#ifndef ENGINE_SETTINGS_MANAGER_H
#define ENGINE_SETTINGS_MANAGER_H

#include "tools/Singleton.h"
#include "tools/MathVector.h"

#include "common/Macros.h"

namespace engine
{
	template<typename Type>
	class Setting
	{
	public:
		inline const Type& Get() const
		{
			return myValue;
		}

	private:

		Setting(const Type& aDefault)
			: myValue(aDefault)
		{

		}

		friend class SettingsManager;
		Type myValue;
	};


	class SettingsManager : public tools::Singleton<SettingsManager>
	{
	public:
		Setting<tools::V2ui> myWindowSize = tools::V2ui(1920, 1080);
		Setting<float> myFOV = TORAD(90);

		void LoadOrDefaultImGuiStyle();
		void SaveImGuiStyle();
	};

}

#endif