
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


	LockedResource<RootCollection> AllRoots()
	{
		static std::unordered_map<std::thread::id, TimeTree*> roots;
		static std::mutex mutex;
		return LockedResource<RootCollection>(mutex, roots);
	}

	class TimeTreeRoot
	{
	public:
		TimeTreeRoot()
		{
			AllRoots().Get().emplace(std::this_thread::get_id(), &myTimeTree);
		}
		~TimeTreeRoot()
		{
			LockedResource<RootCollection> handle = AllRoots();
			handle.Get().erase(handle.Get().find(std::this_thread::get_id()));
		}

		TimeTree* Get()
		{
			return &myTimeTree;
		}


	private:
		TimeTree myTimeTree;
	};


	TimeTree* GetTimeTreeRoot()
	{
		thread_local TimeTreeRoot threadLocalTimeTree;
		return threadLocalTimeTree.Get();
	}

	void PushTimeStamp(const char* aName)
	{
		TimeTree* parent = nullptr;
		TimeTree* child = nullptr;
		if (GetTimeStack().empty())
		{
			GetTimeTreeRoot()->myName = aName;
			child = GetTimeTreeRoot();
		}
		else
		{
			parent = GetTimeStack().top();
		}
		if (parent)
		{
			for (auto& i : parent->myChildren)
			{
				if (i->myName == aName)
				{
					child = i;
				}
			}
		}
		if (!child)
		{
			child = new TimeTree();
			child->myParent = parent;
			parent->myChildren.push_back(child);
			child->myName = aName;
		}
		++child->myCallCount;
		child->myTimeStamp = GetTotalTime();
		GetTimeStack().push(child);
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
		for (std::pair<std::thread::id, TimeTree*> root : AllRoots().Get())
		{
			FlushTimeTreeNode(root.second);
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

