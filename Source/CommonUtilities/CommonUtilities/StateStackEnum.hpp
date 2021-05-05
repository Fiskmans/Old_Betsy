#pragma once

namespace CommonUtilities
{
	enum class StateType 
	{
		MainMenu, 
		OptionsMenu,
		InGame,
		InGameMenu
	};
}

#define STATE_TYPE CommonUtilities::StateType
#define STACK_INIT_SIZE 5