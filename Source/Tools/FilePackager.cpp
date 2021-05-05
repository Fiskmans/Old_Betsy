#include <pch.h>
#include "FilePackager.h"
#include <fstream>


void FilePackager::Release(streamWrapper* aWrapper)
{
	for (auto& i : myFileBuffer)
	{
		if (i.second == aWrapper)
		{

		}
	}
}
