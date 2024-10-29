// Copyright 2020-2022 Russ 'trdwll' Treadwell <trdwll.com>. All Rights Reserved.

#include "SteamBridge.h"

#include "Steam.h"

#include <HAL/FileManager.h>
#include <HAL/PlatformProcess.h>
#include <Misc/Paths.h>
#include <Modules/ModuleManager.h>

#ifndef STEAM_SDK_INSTALLED
#error Steam SDK not located! Expected to be found in Engine/Source/ThirdParty/Steamworks/{SteamVersion}
#endif  // STEAM_SDK_INSTALLED

#define LOCTEXT_NAMESPACE "FSteamBridgeModule"

void FSteamBridgeModule::StartupModule()
{
	const FString STEAM_SDK_ROOT_PATH(TEXT("Binaries/ThirdParty/Steamworks"));

#if PLATFORM_WINDOWS
#if PLATFORM_64BITS
	FString STEAM_SDK_PATH = FPaths::EngineDir() / STEAM_SDK_ROOT_PATH / STEAM_SDK_VER_PATH / TEXT("Win64/");
#else
	FString STEAM_SDK_PATH = FPaths::EngineDir() / STEAM_SDK_ROOT_PATH / STEAM_SDK_VER_PATH / TEXT("Win32/");
#endif  //PLATFORM_64BITS
	FString SDKBinaries = FPaths::Combine(*STEAM_SDK_PATH, FString::Printf(TEXT("steam_api%s.dll"), PLATFORM_64BITS ? *FString("64") : *FString("")));
	SteamLibSDKHandle = FPlatformProcess::GetDllHandle(*(SDKBinaries));
#elif PLATFORM_LINUX
#if PLATFORM_64BITS
	FString STEAM_SDK_PATH = FPaths::EngineDir() / STEAM_SDK_ROOT_PATH / STEAM_SDK_VER_PATH / TEXT("x86_64-unknown-linux-gnu/");
#else
	FString STEAM_SDK_PATH = FPaths::EngineDir() / STEAM_SDK_ROOT_PATH / STEAM_SDK_VER_PATH / TEXT("i686-unknown-linux-gnu/");
#endif  //PLATFORM_64BITS
	FString SDKBinaries = FPaths::Combine(*STEAM_SDK_PATH, "libsteam_api.so");
	SteamLibSDKHandle = FPlatformProcess::GetDllHandle(*(SDKBinaries));
#elif PLATFORM_MAC
	FString STEAM_SDK_PATH = FPaths::EngineDir() / STEAM_SDK_ROOT_PATH / STEAM_SDK_VER_PATH / TEXT("Mac/");
	FString SDKBinaries = FPaths::Combine(*STEAM_SDK_PATH, "libsteam_api.dylib");
	SteamLibSDKHandle = FPlatformProcess::GetDllHandle(*(SDKBinaries));
#endif
}

void FSteamBridgeModule::ShutdownModule()
{
	if (UObjectInitialized())
	{
		SteamAPI_Shutdown();
		SteamGameServer_Shutdown();

		if (SteamLibSDKHandle != nullptr)
		{
			FPlatformProcess::FreeDllHandle(SteamLibSDKHandle);
		}
	}
}

bool FSteamBridgeModule::Tick(float DeltaTime)
{
	SteamAPI_RunCallbacks();
	SteamGameServer_RunCallbacks();

	return true;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSteamBridgeModule, SteamBridge)
