#include "ULUEInstance.h"
#include "UltralightUE.h"

namespace ultralightue
{
    void ULUEInstance::SetupUltralight()
    {
        UE_LOG(LogTemp, Log, TEXT("ULUEInstance: Instance ready. waiting for Ultralight to be ready..."));
        if (FUltralightUEModule::IsAvailable())
        {
            if(FUltralightUEModule::Get().StartupUltralight())
            {
                UE_LOG(LogTemp, Log, TEXT("ULUEInstance: Ultralight is ready!"));
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("ULUEInstance: Failed to start Ultralight!"));
            }
        }
    }
}