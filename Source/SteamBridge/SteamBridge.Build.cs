// Copyright 2020-2022 Russ 'trdwll' Treadwell <trdwll.com>. All Rights Reserved.

using UnrealBuildTool;

public class SteamBridge : ModuleRules
{
	public SteamBridge(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Engine" });
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "Projects",
            "Steamworks",
            "OnlineSubsystem",
            "OnlineSubsystemUtils",
            "OnlineSubsystemSteam",
            "SteamShared"
        });

        AddEngineThirdPartyPrivateStaticDependencies(Target, "Steamworks");

        if (Target.Type == TargetRules.TargetType.Editor)
        {
            PrivateDependencyModuleNames.AddRange(new string[] { "Settings" });
        }
    }
}
