#pragma once
class GraphAction
{
public:
	virtual void Do() = 0;
	virtual void Undo() = 0;
};

