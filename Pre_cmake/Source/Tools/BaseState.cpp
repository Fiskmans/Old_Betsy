#pragma once
#include <pch.h>
#include "BaseState.h"

BaseState::BaseState(bool shouldDeleteOnPop) :
	myShouldDeleteOnPop(shouldDeleteOnPop)
{
	myIsMain = false;
	myUpdateThrough = false;
	myDrawThrough = false;
}
BaseState::~BaseState()
{

}