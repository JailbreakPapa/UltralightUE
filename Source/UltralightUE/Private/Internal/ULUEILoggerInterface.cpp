#include "ULUEILoggerInterface.h"

namespace ultralightue
{
    void ULUEILoggerInterface::LogMessage(ultralight::LogLevel log_level, const ultralight::String& message)
    {
        // Log the message using the Unreal Engine logging system
        switch (log_level)
        {
            case ultralight::LogLevel::Error:
                UE_LOG(LogUltralightUE, Error, TEXT("%s"), *FString(message.c_str()));
                break;
            case ultralight::LogLevel::Warning:
                UE_LOG(LogUltralightUE, Warning, TEXT("%s"), *FString(message.c_str()));
                break;
            case ultralight::LogLevel::Info:
                UE_LOG(LogUltralightUE, Log, TEXT("%s"), *FString(message.c_str()));
                break;
            default:
                break;
        }
    }
}