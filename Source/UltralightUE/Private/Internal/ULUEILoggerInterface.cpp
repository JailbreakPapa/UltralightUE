#include "ULUEILoggerInterface.h"
#include "ULUELogging.h"

namespace ultralightue
{
    void ULUEILoggerInterface::LogMessage(ultralight::LogLevel log_level, const ultralight::String& message)
    {
        FString Msg = UTF8_TO_TCHAR(message.utf8().data());
        switch (log_level)
        {
            case ultralight::LogLevel::Error:
                UE_LOG(LogUltralightUE, Error, TEXT("%s"), *Msg);
                break;
            case ultralight::LogLevel::Warning:
                UE_LOG(LogUltralightUE, Warning, TEXT("%s"), *Msg);
                break;
            case ultralight::LogLevel::Info:
                UE_LOG(LogUltralightUE, Log, TEXT("%s"), *Msg);
                break;
            default:
                break;
        }
    }
}
