
#include <stack>
#include <cassert>

#include "TimeHelper.h"

namespace tools
{
	std::stack<TimeTree*>& GetTimeStack()
	{
		static std::stack<TimeTree*> out;
		return out;
	}

	std::unordered_map<std::thread::id, TimeTree*>& GetAllRoots()
	{
		static std::unordered_map<std::thread::id, TimeTree*> roots;
		return roots;
	}

	TimeTree* GetTimeTreeRoot()
	{
		//TODO #1 this doesnt seem safe...
		auto& roots = GetAllRoots();
		auto it = roots.find(std::this_thread::get_id());
		if (it == roots.end())
		{
			roots[std::this_thread::get_id()] = new TimeTree();
		}
		return roots[std::this_thread::get_id()];
	}

	void PushTimeStamp(const char* aName)
	{
		TimeTree* current = nullptr;
		TimeTree* parent = nullptr;
		if (GetTimeStack().empty())
		{
			GetTimeTreeRoot()->myName = aName;
			parent = GetTimeTreeRoot();
		}
		else
		{
			current = GetTimeStack().top();
		}
		if (current)
		{
			for (auto& i : current->myChildren)
			{
				if (i->myName == aName)
				{
					parent = i;
				}
			}
		}
		if (!parent)
		{
			parent = new TimeTree();
			parent->myParent = current;
			current->myChildren.push_back(parent);
			parent->myName = aName;
		}
		++parent->myCallCount;
		parent->myTimeStamp = GetTotalTime();
		GetTimeStack().push(parent);
	}
	float PopTimeStamp()
	{
		TimeTree* current = GetTimeStack().top();
		current->myTime += GetTotalTime() - current->myTimeStamp;
		if (current->myChildren.empty())
		{
			current->myCovarage = 1.f;
		}
		else
		{
			float covered = 0.f;
			for (auto& i : current->myChildren)
			{
				covered += i->myTime;
			}
			current->myCovarage = covered / current->myTime;
		}

		if (GetTimeStack().empty())
		{
			assert(false && "PoptimeStamp was called without first calling push time stamp or was called to many times");
		}
		GetTimeStack().pop();
		return current->myTime;
	}

	void FlushTimeTreeNode(TimeTree* aTree)
	{
		aTree->myTime = 0;
		aTree->myCallCount = 0;
		for (auto& node : aTree->myChildren)
		{
			FlushTimeTreeNode(node);
		}
	}

	void FlushTimeTree()
	{
		for (auto& i : GetAllRoots())
		{
			FlushTimeTreeNode(i.second);
		}
	}



	ScopeDiagnostic::ScopeDiagnostic(const char* aName)
	{
		PushTimeStamp(aName);
	}
	ScopeDiagnostic::~ScopeDiagnostic()
	{
		PopTimeStamp();
	}
}

