#if defined _WIN32 || defined _WIN64
#include <Windows.h>

#define ULTRALIGHTUE_EXPORT __declspec(dllexport)
#else
#include <stdio.h>
#endif

#ifndef ULTRALIGHTUE_EXPORT
#define ULTRALIGHTUE_EXPORT
#endif

ULTRALIGHTUE_EXPORT void UltralightUEFunction()
{
#if defined _WIN32 || defined _WIN64
	MessageBox(NULL, TEXT("Loaded Ultralight Dll(s) from Pre-built binaries."), TEXT("UltralightUE"), MB_OK);
#else
	printf("Loaded Ultralight Dll(s) from Pre-built binaries.");
#endif
}