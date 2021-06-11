#pragma once

#pragma warning(disable : 26812) // Prefer scoped enum


#include <Macros.h>

//Windows
#define WIN32_LEAN_AND_MEAN
#include <ws2tcpip.h>
#include <windows.h>

//DirectX
#include <d3dcommon.h>
#include <d3d11.h>

//Std
#include <string>
#include <vector>
#include <variant>
#include <future>
#include <array>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <typeindex>
#include <algorithm>
#include <functional>
#include <fstream>
#include <mutex>
#include <stack>
#include <queue>
#include <filesystem>
#include <bitset>
#include <concepts>

//C
#include <cassert>
#include <cmath>

//CommonUtilities
#include "CommonUtilities\Matrix.hpp"
#include "CommonUtilities\Vector.hpp"
#include "CommonUtilities\ObjectPool.h"

//TOOLS
#include "Tools\Logger.h"
#include "Tools\FiskJSON.h"
#include "Tools\MetricValue.h"
#include "Tools\TimeHelper.h"
#include "Tools\FileHelpers.h"
#include "Tools\StringManipulation.h"
#include "Tools\PostMaster.hpp"
#include "Tools\Observer.hpp"
#include "Tools\Random.h"
#include "Tools\ExecuteOnDestruct.h"

#if USEFILEWATHCER
#include "Tools\FileWatcher.h"
#endif

//Typedefs
#include <typedefs.h>

//Gamla Bettan
#include "GamlaBettan\Asset.h"
#include "GamlaBettan\DebugDrawer.h"
#include "GamlaBettan\WindowControl.h"
#include "GamlaBettan\ComponentManager.h"

//Literals
#include "Precompiled\Literals.h"

//Conditionals
#if USEIMGUI
#include "GamlaBettan\DebugTools.h"
#include "Tools\ImGuiHelpers.h"
#endif

#ifdef _DEBUG
#include <DebugTools.h>
#endif

