#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <unordered_map>

#include "tools/Logger.h"
#include "tools/Macros.h"
#include "tools/Time.h"

#if WIN32

#define NOMINMAX
#include <ShObjIdl.h>
#include <WinUser.h>
#include <windows.h>

#endif

#include <cstdio>
#include <cstring>
#include <time.h>
#include <csignal>

namespace fisk::tools
{
    LoggerType Filter  = Type::All;
    LoggerType Halting = Type::None;

    std::unordered_map<LoggerType, std::string> FileMapping;
    std::unordered_map<LoggerType, char> ColorMapping;
    std::unordered_map<std::string, std::ofstream> OpenFiles;

    float RapportTimeStamp = GetTotalTime();

    struct LoggerNode
    {
        bool myIsOpen  = true;
        size_t myCount = 0;
        std::unordered_map<std::string, LoggerNode*> mySubNodes;
    };

    std::unordered_map<LoggerType, LoggerNode> Roots;

#ifdef WIN32
    HWND window             = NULL;
    ITaskbarList3* pTaskbar = nullptr;
    HICON errorIcon;
    HICON warningIcon;
#else
    constexpr char FOREGROUND_RED    = 0b0001;
    constexpr char FOREGROUND_GREEN  = 0b0010;
    constexpr char FOREGROUND_BLUE   = 0b0100;

#endif

    const char DefaultConsoleColor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    const char* GetSeverity(LoggerType aType)
    {
        switch (aType)
        {
        case Type::SystemInfo:
        case Type::Info:
            return "Info";
        case Type::SystemError:
        case Type::Error:
            return "Error";
        case Type::SystemWarning:
        case Type::Warning:
            return "Warning";
        case Type::SystemVerbose:
        case Type::Verbose:
            return "Verbose";
        case Type::SystemCrash:
            return "Crash";
        default:
            return "Unkown";
        }
    }

    std::recursive_mutex ourMutex;
    static std::vector<std::pair<LoggerType, std::pair<std::string, char>>> ImguiLog;

    void Rapport(LoggerType aType, const std::string& aFile, const size_t aLine, const std::string& aError,
                 const std::vector<std::string>& aArguments)
    {
        std::string toPrint = "[" + aFile + ":" + std::to_string(aLine) + "]: " + aError;

        for (size_t i = 0; i < sizeof(LoggerType) * CHAR_BIT; i++)
        {
            if (BIT(i) & aType)
            {
                auto& rootNode = Roots[BIT(i)];
                ++rootNode.myCount;

                if (rootNode.mySubNodes.count(aFile) == 0)
                {
                    rootNode.mySubNodes[aFile] = new LoggerNode();
                }

                LoggerNode* currentNode = rootNode.mySubNodes[aFile];
                currentNode->myCount++;

                if (currentNode->mySubNodes.count(aError) == 0)
                {
                    currentNode->mySubNodes[aError] = new LoggerNode();
                }
                currentNode = currentNode->mySubNodes[aError];
                currentNode->myCount++;

                for (const std::string& arg : aArguments)
                {
                    toPrint += ", " + arg;

                    if (currentNode->mySubNodes.count(arg) == 0)
                    {
                        currentNode->mySubNodes[arg] = new LoggerNode();
                    }
                    currentNode = currentNode->mySubNodes[arg];
                    currentNode->myCount++;
                }
            }
        }

        Log(aType, toPrint);
    }

    void Log(LoggerType aType, const std::string& aMessage)
    {
        std::lock_guard lock(ourMutex);

#if WIN32
        static HANDLE consoleHandle;
        static bool first = true;
        if (first)
        {
            consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
            first         = true;
        }

        if (pTaskbar)
        {
            if (aType & Type::AnyError)
            {
                pTaskbar->SetOverlayIcon(window, errorIcon, L"Error");
            }

            if (aType & Type::AnyWarning)
            {
                pTaskbar->SetOverlayIcon(window, warningIcon, L"Warning");
            }
		}
#endif

        char color = DefaultConsoleColor;
        bool reset = false;
        if ((aType != 0) && ((aType & (aType - 1)) == 0)) // is power of 2
        {
            if (FileMapping.count(aType) != 0)
            {
                char buffer[256];
                memset(buffer, '\0', 256);
                std::time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                ctime_s(buffer, 256, &time);
                if (*buffer != '\0')
                {
                    buffer[strlen(buffer) - 1] = '\0';
                }
                OpenFiles[FileMapping[aType]] << "[" << buffer << "] [" << GetSeverity(aType) << "] " << aMessage
                                              << std::endl;
            }

            if (ColorMapping.count(aType) != 0)
            {
                color = ColorMapping[aType];
#if WIN32
                SetConsoleTextAttribute(consoleHandle, color);
#else
                ::printf("\033[38;5;%u;", color);
#endif
                reset = true;
            }

            ImguiLog.push_back(std::make_pair(aType, std::make_pair(aMessage, color)));
        }
        else
        {
            LOG_SYS_ERROR("Logging using multiple filters is not supported");
        }
        if ((aType & Filter) != 0)
        {
            std::cout << aMessage << std::endl;
        }

        if (reset)
		{
#if WIN32
            SetConsoleTextAttribute(consoleHandle, DefaultConsoleColor);
#else
            ::puts("\033[0");
#endif
        }

        if ((aType & Halting) != 0)
        {
#if WIN32
            SetActiveWindow(GetConsoleWindow());
            MessageBoxA(NULL, aMessage.c_str(), "A halting error occurred", MB_OK);
#else
            std::signal(SIGINT, SIG_DFL);
#endif
        }
    }

    void SetFilter(LoggerType aFilter)
    {
        if ((aFilter & Halting) != Halting)
        {
            LOG_SYS_ERROR("Trying to set logger filter which does not encompass all halting types");
            return;
        }
        Filter = aFilter;
    }

    void SetHalting(LoggerType aFilter)
    {
        if ((aFilter & Filter) != aFilter)
        {
            LOG_SYS_ERROR("Trying to set logger halting which is not encompassed completely by filter");
            return;
        }
        Halting = aFilter;
    }
    void Map(LoggerType aMessageType, std::string aOutputFile)
    {
        aOutputFile = "logs/" + aOutputFile + ".log";
        for (LoggerType i = 0; i < sizeof(LoggerType) * CHAR_BIT; i++)
        {
            LoggerType bit = aMessageType & (1ULL << i);
            if (bit != 0)
            {
                LoggerType messageType = static_cast<Type>(bit);

                if (FileMapping.count(messageType) != 0)
                {
                    LOG_SYS_ERROR("Type is already mapped: ", std::to_string(messageType));
                    return;
                }

                FileMapping[messageType] = aOutputFile;
                if (OpenFiles.count(aOutputFile) == 0)
                {
                    if (!std::filesystem::exists("logs/"))
                    {
                        std::filesystem::create_directories("logs/");
                    }

                    OpenFiles[aOutputFile].open(aOutputFile); // default construct and open
                    if (!OpenFiles[aOutputFile].good())
                    {
                        LOG_SYS_ERROR("Could not open file for logging", aOutputFile);
                    }
                }
            }
        }
    }

    void UnMap(LoggerType aMessageType)
    {
        for (LoggerType i = 0; i < sizeof(LoggerType) * CHAR_BIT; i++)
        {
            LoggerType bit = aMessageType & (1ULL << i);
            if (bit != 0)
            {
                LoggerType messageType = static_cast<Type>(bit);

                if (FileMapping.count(messageType) == 0)
                {
                    LOG_SYS_ERROR("Trying to unmap type which is not mapped: ", std::to_string(messageType));
                    continue;
                }
                LOG_SYS_INFO("Unmapping: " + std::to_string(messageType) + " from: " + FileMapping[messageType]);

                std::string file = FileMapping[messageType];
                FileMapping.erase(messageType);
                for (auto& fileMap : FileMapping)
                {
                    if (fileMap.second == file)
                    {
                        return;
                    }
                }
                OpenFiles[file].close();
                OpenFiles.erase(file); // destruct
            }
        }
    }
    void SetColor(LoggerType aMessageType, char aColor)
    {
        for (LoggerType i = 0; i < sizeof(LoggerType) * CHAR_BIT; i++)
        {
            LoggerType bit = aMessageType & (1ULL << i);
            if (bit != 0)
            {
                LoggerType messageType = static_cast<Type>(bit);

                ColorMapping[messageType] = aColor;
            }
        }
    }

#if WIN32
    void SetupIcons(void* aHWND, const std::string& aErrorIcon, const std::string& aWarningIcon)
    {
        window     = reinterpret_cast<HWND>(aHWND);
        HRESULT hr = CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_ITaskbarList3,
                                      reinterpret_cast<void**>(&pTaskbar));
        if (FAILED(hr))
        {
            std::cerr << "Failed to set up taskbar instance" << std::endl;
            return;
        }

        errorIcon = static_cast<HICON>(
            ::LoadImage(NULL, aErrorIcon.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTCOLOR));
        if (errorIcon == NULL)
        {
            std::cerr << "Failed to load error icon" << std::endl;
            pTaskbar->Release();
            pTaskbar = nullptr;
            return;
        }

        warningIcon = static_cast<HICON>(
            ::LoadImage(NULL, aWarningIcon.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTCOLOR));
        if (warningIcon == NULL)
        {
            std::cerr << "Failed to load warning icon" << std::endl;
            return;
        }
    }
#endif

    void Shutdown()
    {
#if WIN32
        if (warningIcon)
            DestroyIcon(warningIcon);

        if (errorIcon)
            DestroyIcon(errorIcon);

        if (pTaskbar)
            pTaskbar->Release();

        OpenFiles.clear(); // flush all files
#endif
    }
} // namespace fisk::tools
