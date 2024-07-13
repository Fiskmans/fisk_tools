#if _WIN32 || _WIN64
#define FISK_PLATFORM_WINDOWS (1)
#else
#define FISK_PLATFORM_WINDOWS (0)
#endif

#ifdef __linux__
#define FISK_PLATFORM_LINUX (1)
#else
#define FISK_PLATFORM_LINUX (0)
#endif