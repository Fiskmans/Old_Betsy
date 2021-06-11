#pragma once

//Constants
#define PI 3.14159265359f
#define TAU (2.0 * PI)
// sqrt(3)
#define CUBEHALFSIZETOENCAPSULATINGSPHERERADIUS 1.73f
#define PFSTRING "%s"
#define PFSIZET "%zu"
#define PFFLOAT "%.f"
#define PFINT "%d"
#define PFUINT "%u"

//Values
#define SHADOWRESOLUTION 246
#define ENVIRONMENTSHADOWRESOLUTION 8192
#define ENVIRONMENCAMEREASIZE 10_m
#define DECALRESOLUTION 256
#define NUMBEROFLODLEVELS 3
#define NUMBEROFPOINTLIGHTS 1
#define NUMBEROFANIMATIONBONES 64
#define MAXNUMBEROFANIMATIONSONSCREEN 256
#define MAX_NUM_BONES_PER_VEREX 15
#define MODELSAMOUNTOFCUSTOMDATA 64
#define NEARPLANE 1.f
#define FARPLANE 10000.f
#define SHADOWNEARPLANE 10.f
#define SHADOWFARPLANE 3000.f
#define CULLINGFRUSTOMREFRESHRATE 0.75f
#define LATENTCULLINGFRUSTUMMULTIPLIER 5.f
#define PLAYERPOSITIONVALIDATIONINTERVAL 0.2f
#define MAXDEBUGLINES 150000
#define STANDARDMARG (1e-10)
#define METRICFILE "Metrics.h"
#define MAXUSERNAMELENGTH 128
#define AMOUNTOFDAYS 15


//Flags
#define VISUALIZEBOUNDINGSPHERES false
#define VISUALIZESPHERETOPLANEVOLUMECHECK false
#define USEMEMORYCONTROL false
#define WARNUSINGLODLEVELS true
#define USEFILEWATHCER true
#define DEMOSCENE false
#define BAKEVALUES false
#define ENABLEDEBIGDEVICE false
#define USELOGGER true
#define USEIMGUI true
#define DEBUGBONES true
#define USEAUDIO true
#define ENABLENUMPADLEVELSELECT false
#define BOOTUPDIAGNOSTIC USEIMGUI
#define STREAMANIMATIONS true
#define TRACKPERFORMANCE true
#define USEDEBUGLINES true
#define TRACKCOMPONENTUSAGE true
#define COPYDLLS true
#define KILLWHENOUTOFBOUNDS true
#define ENABLEBLOOM false
#define ENABLESSAO true
#define EDITFARMBOUNDS false
#define TRACKGFXMEM true

#define USEITERATABLEQUEUE USEIMGUI

#define USEFULLSCREEN true
#define ESCPACEQUITSGAME false

#ifdef _DEBUG
#define DIRECTTOGAME false
#define PLAYINTRO false
#else
#define DIRECTTOGAME false
#define PLAYINTRO true
#endif //_DEBUG


#define ENABLERENDERPASSITERATIONS (USEIMGUI && true)



//Funtions
#define CAST(type, value) (static_cast<type>(value))
#define ENUM_CAST(value) (static_cast<int>(value))

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define CLOSEENUF(a, b) (static_cast<double>(abs(a - b)) <= STANDARDMARG)
#define CLOSEENUFCUSTOM(a, b, margin) (abs(a - b) <= margin)

#define SAFE_DELETE(pointer) if(pointer) { delete (pointer); (pointer) = nullptr; }
#define SAFE_RELEASE(pointer) if(pointer) { pointer->Release(); pointer = nullptr; }

#define SAFE_DELETE_ARRAY(pointer) if(pointer) { delete[] (pointer); (pointer) = nullptr; }

#define CYCLIC_ERASE(vector, index) if(index >= 0 && index < vector.size()) { if(vector.size() > 0) { vector.at(index) = vector.at(vector.size() - 1); vector.pop_back(); } } else { assert(false); }

#define NAME_OF_CLASS(type) (typeid(type).name() + 6)

#define BIT(x) (1ULL << x)

#define LERP(a, b, val) ((a) * (1 - (val)) + (val) * (b))
#define INVERSELERP(a, b, val) (((val) - (a)) / ((b) - (a)))

#define CLAMP(low, high, value) ((value) < (low) ? (low) : ((high) < (value) ? (high) : (value)))

#ifndef ZEROMEMORY
#define ZEROMEMORY(adr, size) memset(adr, 0, size)
#endif // !ZeroMemory

#define WIPE(item) ZEROMEMORY(&item, sizeof(item))

#define STRING(arg) #arg
#define STRINGVALUE(arg) STRING(arg)

#ifdef _DEBUG
#define NAMETHREAD(name) SetThreadDescription(GetCurrentThread(), name);
#else
#define NAMETHREAD(name) ((void*)0);
#endif // _DEBUG


#if TRACKPERFORMANCE
#include <TimeHelper.h>   //Min VS gnäller så jävla hårt, är lite jobbigt att halva kodbasen har röda squigglies
#define PERFORMANCETAG(name) auto _ = Tools::ScopeDiagnostic(name);
#else
#define PERFORMANCETAG(name) ((void*)0);
#endif


#define TODEG(arg) ((arg) * 57.2957795131f)
#define TORAD(arg) ((arg) * 0.01745329251f)

#if BAKEVALUES
#include "../../Game/Metrics.h"
#define GAMEMETRIC(type,name,lookup,defaultValue) type name = type(lookup);
#else
#define GAMEMETRIC(type,name,lookup,defaultValue) MetricValue<type> name = MetricValue<type>(#lookup,defaultValue)
#endif