
#include "tools/Trace.h"

#include <cassert>
#include <shared_mutex>
#include <sstream>

namespace fisk::tools
{

	std::unordered_map<std::thread::id, std::unique_ptr<PerformanceTracer>> PerformanceTracer::ourInstances;
	std::mutex PerformanceTracer::ourRootMutex;
	Trace PerformanceTracer::ourRoot{ "<root>" };

	Trace::Trace(std::string aTag)
		: myTag(aTag)
		, myTimeSpent{0}
		, myTimesTraced{0}
	{
	}

	Trace& Trace::FindOrMake(std::string aTag)
	{
		for (Trace& child : myChildren)
		{
			if (child.myTag == aTag)
				return child;
		}

		myChildren.emplace_back(aTag);
		return myChildren.back();
	}

	PerformanceTracer& PerformanceTracer::GetInstance()
	{
		using ReadLock = std::shared_lock<std::shared_mutex>;
		using WriteLock = std::unique_lock<std::shared_mutex>;
		static std::shared_mutex mutex;

		std::thread::id thisId = std::this_thread::get_id();

		{
			ReadLock readLock(mutex);
			
			decltype(ourInstances)::iterator it = ourInstances.find(thisId);
			if (it != ourInstances.end())
				return *it->second;
		}

		std::unique_ptr<PerformanceTracer> tracer = std::make_unique<PerformanceTracer>();

		PerformanceTracer& raw = *tracer;
		{
			WriteLock writeLock(mutex);

			ourInstances.insert({ thisId, std::move(tracer) });

			return raw;
		}
	}

	PerformanceTracer::PerformanceTracer()
	{
		std::stringstream ss;
		ss << "thread: " << std::this_thread::get_id();

		{
			std::lock_guard lock(ourRootMutex);
			myRoot = & ourRoot.FindOrMake(ss.str());
		}

		myCurrentScope = myRoot;
	}

	PerformanceTracer::~PerformanceTracer()
	{
		assert(myActive.empty());
	}

	PerformanceTracer::ScopeHandle PerformanceTracer::BeginInternal(std::string aTag)
	{
		Trace& next = myCurrentScope->FindOrMake(aTag);

		myActive.emplace(ActiveTrace(&next, myCurrentScope));
		myCurrentScope = &next;
		
		return { this };
	}

	PerformanceTracer::ScopeHandle PerformanceTracer::Begin(std::string aTag)
	{
		return GetInstance().BeginInternal(aTag);
	}

	const std::vector<Trace> PerformanceTracer::GetRoots()
	{
		std::lock_guard lock(ourRootMutex);
		return ourRoot.myChildren;
	}

	void PerformanceTracer::End()
	{
		assert(!myActive.empty());

		myCurrentScope = myActive.top().Apply();
		myActive.pop();
	}
	
	PerformanceTracer::ActiveTrace::ActiveTrace(Trace* aTrace, Trace* aOldTrace)
		: myTrace(aTrace)
		, myOldTrace(aOldTrace)
		, myStart(TracingClock::now())
	{
	}

	Trace* PerformanceTracer::ActiveTrace::Apply()
	{
		myTrace->myTimeSpent += (TracingClock::now() - myStart);
		myTrace->myTimesTraced++;

		return myOldTrace;
	}
	
	PerformanceTracer::ScopeHandle::ScopeHandle(PerformanceTracer* aOwner)
		: myOwner(aOwner)
	{
	}

	PerformanceTracer::ScopeHandle::~ScopeHandle()
	{
		myOwner->End();
	}
}