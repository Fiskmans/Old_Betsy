#pragma once

class ComponentSystemBase
{
	virtual ~ComponentSystemBase() = default;
};

template<class T>
class ComponentSystem
	: public ComponentSystemBase
{


};

