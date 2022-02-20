#pragma once

typedef void* NodeDataPtr;

#define HEAVY_NODE_DATA_DEBUG
#ifdef HEAVY_NODE_DATA_DEBUG
#include <assert.h>
#endif
namespace NodeData
{
	template <class Type>
	Type Get(void* someData)
	{
#ifdef HEAVY_NODE_DATA_DEBUG
		assert(someData);
#endif
		if (!someData)
		{
			return Type(0);
		}
		return *((Type*)someData);
	}
}

// Helpers macros to generate 32-bits encoded colors
#define COL32_R_SHIFT    0
#define COL32_G_SHIFT    8
#define COL32_B_SHIFT    16
#define COL32_A_SHIFT    24
#define COL32_A_MASK     0xFF000000
#define COL32(R,G,B,A)    (((unsigned int)(A)<<COL32_A_SHIFT) | ((unsigned int)(B)<<COL32_B_SHIFT) | ((unsigned int)(G)<<COL32_G_SHIFT) | ((unsigned int)(R)<<COL32_R_SHIFT))
#define COL32_WHITE       COL32(255,255,255,255)  // Opaque white = 0xFFFFFFFF
#define COL32_BLACK       COL32(0,0,0,255)        // Opaque black
#define COL32_BLACK_TRANS COL32(0,0,0,0)          // Transparent black = 0x00000000