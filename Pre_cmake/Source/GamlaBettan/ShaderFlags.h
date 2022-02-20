#pragma once

enum ShaderFlags : unsigned char
{
	None = 0,
	HasVertexColors = 1ULL << 0,
	HasUvSets = 1ULL << 1,
	HasBones = 1ULL << 2,

	NumUvSetsOffset = 3,
	UvMask = 1ULL << 3 | 1ULL << 4,

	NumBonesOffset = 5,
	BoneMask = 1ULL << 5 | 1ULL << 6 | 1ULL << 7,
	Count
};
