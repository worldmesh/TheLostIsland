// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TheLostIsland : ModuleRules
{
	public TheLostIsland(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
            "Niagara",
            "Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"TheLostIsland",
			"TheLostIsland/Variant_Platforming",
			"TheLostIsland/Variant_Platforming/Animation",
			"TheLostIsland/Variant_Combat",
			"TheLostIsland/Variant_Combat/AI",
			"TheLostIsland/Variant_Combat/Animation",
			"TheLostIsland/Variant_Combat/Gameplay",
			"TheLostIsland/Variant_Combat/Interfaces",
			"TheLostIsland/Variant_Combat/UI",
			"TheLostIsland/Variant_SideScrolling",
			"TheLostIsland/Variant_SideScrolling/AI",
			"TheLostIsland/Variant_SideScrolling/Gameplay",
			"TheLostIsland/Variant_SideScrolling/Interfaces",
			"TheLostIsland/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
