#pragma once

enum ShaderFlags: unsigned long long
{
	None = 0,
	HasVertexColors = 1ULL << 0,
	HasUvSets = 1ULL << 1,
	HasBones = 1ULL << 2,


	NumBonesOffset = 32,
	BoneMask = 1ULL << 32 | 1ULL << 33 | 1ULL << 34 | 1ULL << 35,
	Count
};
