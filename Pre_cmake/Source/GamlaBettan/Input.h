#pragma once


class Input : public CommonUtilities::Singleton<Input>, public Observer
{
public:
	Input();

	V2f MousePos();
	FRay MouseRay();

	virtual void RecieveMessage(const Message& aMessage) override;

private:
	V2f myLastMousePos;

};

