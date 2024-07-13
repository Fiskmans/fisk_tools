#ifndef FISK_TOOLS_TIME_HELPER_H
#define FISK_TOOLS_TIME_HELPER_H

#include "tools/LockedResource.h"

#include <chrono>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace fisk::tools
{

    class ScopeDiagnostic
    {
      public:
        ScopeDiagnostic(const char* aName);
        ~ScopeDiagnostic();
    };

    struct TimeTree
    {
        float myTime;
        float myCovarage;
        float myTimeStamp;
        size_t myCallCount;
        const char* myName;
        TimeTree* myParent = nullptr;
        std::vector<TimeTree*> myChildren;
    };

    using RootCollection = std::unordered_map<std::thread::id, TimeTree*>;

    inline float GetTotalTime()
    {
        long long now = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now())
                            .time_since_epoch()
                            .count();
        static long long CreationTime = now;
        return float((now - CreationTime) / double(1000.0 * 1000.0));
    }

    inline std::unordered_map<std::string, float>& GetOpenDiagnostics()
    {
        static std::unordered_map<std::string, float> out;
        return out;
    }

    inline void StartDiagnostic(const std::string& aKey) noexcept
    {
        GetOpenDiagnostics()[aKey] = GetTotalTime();
    }

    inline float EndDiagnostic(const std::string& aKey) noexcept
    {
        return GetTotalTime() - GetOpenDiagnostics()[aKey];
    }

    LockedResource<RootCollection> AllRoots();

    TimeTree* GetTimeTreeRoot();
    void PushTimeStamp(const char* aName);
    float PopTimeStamp();
    void FlushTimeTree();
} // namespace fisk::tools

#endif