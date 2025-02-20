// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Shoot : ModuleRules
{
	public Shoot(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "UMG", "OnlineSubsystem", "OnlineSubsystemSteam", "Niagara", "Slate", "SlateCore"/*, "MultiplayerSessions", "OnlineSubsystemUtils", "Steamworks"*/ });
	}
}
