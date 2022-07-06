#ifndef TOOLS_NAME_THREAD_H
#define TOOLS_NAME_THREAD_H

#include <string>
#include <thread>

namespace tools
{
	std::string GetNameOfThread(std::thread::id aThreadId);

	void NameThread(const std::string& aName);
}

#endif