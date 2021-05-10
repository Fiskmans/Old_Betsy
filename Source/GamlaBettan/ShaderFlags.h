#pragma once

enum ShaderFlags : unsigned char
{
	None = 0,
	HasVertexColors = 1ULL << 0,
	HasUvSets = 1ULL << 1,
	HasBones = 1ULL << 2,


	NumBonesOffset = 4,
	BoneMask = 1ULL << 4 | 1ULL << 5 | 1ULL << 6 | 1ULL << 7,
	Count
};
