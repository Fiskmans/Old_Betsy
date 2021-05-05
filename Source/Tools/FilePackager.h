#pragma once
#include <Singleton.hpp>
#define MAXOPENFILES 16


class streamWrapper : public std::istream
{
	friend class FilePackager;
public:
	~streamWrapper();
	operator std::istream&() { return *this; }
private:


};


class FilePackager : public CommonUtilities::Singleton<FilePackager>
{
	friend streamWrapper;
public:
	std::istream Request(const std::string& aFilePath);

private:
	void Release(streamWrapper* aWrapper);

	std::array<std::pair<std::ifstream&, streamWrapper*>, MAXOPENFILES> myFileBuffer;


};

