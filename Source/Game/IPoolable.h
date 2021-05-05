#pragma once

struct IPoolable
{
	virtual void OnReturn() = 0;
	virtual void OnRetrieve() = 0;
	virtual void OnLoad() = 0;
};