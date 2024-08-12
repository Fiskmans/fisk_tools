#pragma once 

#include <memory>
#include <chrono>
#include <vector>
#include <stack>
#include <unordered_map>
#include <thread>
#include <mutex>

namespace fisk::tools
{
	using TracingClock = std::chrono::high_resolution_clock;

	struct Trace
	{
		Trace(std::string aTag);

		Trace& FindOrMake(std::string aTag);

		std::string myTag;
		TracingClock::duration myTimeSpent;
		size_t myTimesTraced;

		std::vector<Trace> myChildren;
	};

	class PerformanceTracer
	{
	public:
		PerformanceTracer();
		~PerformanceTracer();

		class ScopeHandle
		{
		public:
			ScopeHandle(PerformanceTracer* aOwner);
			~ScopeHandle();

		private:
			PerformanceTracer* myOwner;
		};

		static ScopeHandle Begin(std::string aTag);
		static const std::vector<Trace> GetRoots();

	private:
		static PerformanceTracer& GetInstance();


		class ActiveTrace
		{
		public:
			ActiveTrace(Trace* aTrace, Trace* aOldTrace);

			Trace* Apply();

		private:
			Trace* myTrace;
			Trace* myOldTrace;
			TracingClock::time_point myStart;
		};

		friend ScopeHandle;
		ScopeHandle BeginInternal(std::string aTag);
		void End();

		static std::unordered_map<std::thread::id, std::unique_ptr<PerformanceTracer>> ourInstances;
		static std::mutex ourRootMutex;
		static Trace ourRoot;

		Trace* myRoot;
		Trace* myCurrentScope;

		std::stack<ActiveTrace> myActive;
	};
}

#define CONCAT_INNER(a, b) a ## b
#define CONCAT(a, b) CONCAT_INNER(a, b)

#define FISK_TRACE(TAG) fisk::tools::PerformanceTracer::ScopeHandle CONCAT(TraceHandle, __COUNTER__) = fisk::tools::PerformanceTracer::Begin(TAG)