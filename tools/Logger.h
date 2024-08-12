#ifndef FISK_TOOLS_LOGGER_H
#define FISK_TOOLS_LOGGER_H

#include <climits>
#include <string>
#include <vector>

#define LOG(type, message, ...)       fisk::tools::Rapport(type, __FILE__, __LINE__, message, {__VA_ARGS__})

#define LOG_SYS_CRASH(message, ...)   LOG(fisk::tools::Type::SystemCrash, message, __VA_ARGS__)
#define LOG_SYS_ERROR(message, ...)   LOG(fisk::tools::Type::SystemError, message, __VA_ARGS__)
#define LOG_SYS_INFO(message, ...)    LOG(fisk::tools::Type::SystemInfo, message, __VA_ARGS__)
#define LOG_SYS_WARNING(message, ...) LOG(fisk::tools::Type::SystemWarning, message, __VA_ARGS__)
#define LOG_SYS_VERBOSE(message, ...) LOG(fisk::tools::Type::SystemVerbose, message, __VA_ARGS__)
#define LOG_SYS_NETWORK(message, ...) LOG(fisk::tools::Type::SystemNetwork, message, __VA_ARGS__)

#define LOG_INFO(message, ...)        LOG(fisk::tools::Type::Info, message, __VA_ARGS__)
#define LOG_WARNING(message, ...)     LOG(fisk::tools::Type::Warning, message, __VA_ARGS__)
#define LOG_ERROR(message, ...)       LOG(fisk::tools::Type::Error, message, __VA_ARGS__)
#define LOG_VERBOSE(message, ...)     LOG(fisk::tools::Type::Verbose, message, __VA_ARGS__)

namespace fisk::tools
{
    typedef unsigned short LoggerType;

    enum Type : LoggerType
    {
        /// types
        // system
        SystemInfo    = 1LL << 0,
        SystemError   = 1LL << 1,
        SystemCrash   = 1LL << 2,
        SystemWarning = 1LL << 3,
        SystemVerbose = 1LL << 4,
        SystemNetwork = 1LL << 5,

        // game
        Info    = 1LL << 6,
        Warning = 1LL << 7,
        Error   = 1LL << 8,
        Verbose = 1LL << 9,

        /// filters
        // global
        None = 0x0000,
        All  = 0xFFFF,

        // severity
        AnyInfo    = SystemInfo | Info,
        AnyWarning = SystemWarning | Warning,
        AnyError   = SystemCrash | SystemError | Error,

        // type
        AnyGame    = Info | Warning | Error,
        AnySystem  = SystemCrash | SystemError | SystemInfo | SystemWarning,
        AnyVerbose = Verbose | SystemVerbose | SystemNetwork,

        AllGame   = AnyGame | Verbose,
        AllSystem = AnySystem | SystemVerbose
    };

    void Rapport(LoggerType aType, const std::string& aFile, const size_t aLine, const std::string& aError,
                 const std::vector<std::string>& aArguments);
    void Log(LoggerType aType, const std::string& aMessage);
    void SetFilter(LoggerType aFilter);
    void SetHalting(LoggerType aFilter);
    void Map(LoggerType aMessageType, std::string aOutputFile);
    void UnMap(LoggerType aMessageType);
    void SetColor(LoggerType aMessageType, char aColor);

#ifdef WIN32
    void SetupIcons(void* aHWND, const std::string& aErrorIcon, const std::string& aWarningIcon);
#endif

    void Shutdown();
} // namespace fisk::tools
#endif