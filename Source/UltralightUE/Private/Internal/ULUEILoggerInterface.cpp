#include "ULUEILoggerInterface.h"

DEFINE_LOG_CATEGORY(LogUltralightUE);

void ILogInterface::LogMessage(ultralight::LogLevel log_level, const ultralight::String& message)
{
	// Log the message using the Unreal Engine logging system
	switch (log_level)
	{
	case ultralight::LogLevel::Error:
		UE_LOG(LogUltralightUE, Error, TEXT("%hs"), message.utf8().data());
		break;
	case ultralight::LogLevel::Warning:
		UE_LOG(LogUltralightUE, Warning, TEXT("%hs"), message.utf8().data());
		break;
	case ultralight::LogLevel::Info:
		UE_LOG(LogUltralightUE, Log, TEXT("%hs"), message.utf8().data());
		break;
	default:
		break;
	}
}
