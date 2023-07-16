#if defined _WIN32 || defined _WIN64
#define ULTRALIGHTUE_IMPORT __declspec(dllimport)
#elif defined __linux__
#define ULTRALIGHTUE_IMPORT __attribute__((visibility("default")))
#else
#define ULTRALIGHTUE_IMPORT
#endif

ULTRALIGHTUE_IMPORT void UltralightUEFunction();
