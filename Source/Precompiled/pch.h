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

// Std
#pragma warning(push)
#pragma warning(disable : 4244)
#include <string>
#pragma warning(pop)
#include <vector>
#include <variant>
#include <future>
#include <array>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <typeindex>
#include <algorithm>
#include <fstream>
#include <mutex>
#include <stack>
#include <queue>
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
#include <filesystem>
#include <bitset>
#include <cmath>

//C
#include <cassert>

//CommonUtilities
#include <Matrix.hpp>
#include <Vector.hpp>
#include <PostMaster.hpp>
#include <Observer.hpp>
#include <Publisher.hpp>

//TOOLS
#include <Logger.h>
#include <FiskJSON.h>
#include <MetricValue.h>
#include <TimeHelper.h>
#include <FileHelpers.h>
#include <StringManipulation.h>

//Typedefs
#include <typedefs.h>

//Gamla Bettan
#include <ModelInstance.h>
#include <DebugDrawer.h>
#include <PathFinder.h>
#include <Camera.h>
#include <Scene.h>
#include <TextureLoader.h>
#include <WindowControl.h>

//Game
#include "../Game/Item.h"
#include "../Game/Entity.h"
#include "../Game/Mesh.h"
#include "../Game/Light.h"
#include "../Game/ParticleOnDeath.h"
#include "../Game/ParticlesWhileAlive.h"
#include "../Game/Audio.h"
#include "../Game/Collision.h"
#include "../Game/Life.h"
#include "../Game/LifeTime.h"
#include "../Game/Movement3D.h"
#include "../Game/PlayerController.h"
#include "../Game/CharacterInstance.h"
#include "../Game/ParticleOnHit.h"
#include "AssetImportHelpers.h"

//Network
#include "../TShared/CommonNetworkIncludes.h"
#include "../TShared/EasyAssignString.h"

//Literals
#include "../Precompiled/Literals.h"

//Conditionals
#if USEIMGUI
#include <imgui.h>
#include <ImGuiHelpers.h>
#endif

#ifdef _DEBUG
#include <DebugTools.h>
#endif

#if USEFILEWATHCER
#include <FileWatcher.h>
#endif

