// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class LousyMemoryManager : ModuleRules
{
    public LousyMemoryManager(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // Public headers
        PublicIncludePaths.AddRange(
            new string[] {
                
            }
        );

        // Private headers
        PrivateIncludePaths.AddRange(
            new string[] {
                
            }
        );

        // Public dependencies - minimal & clean
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "AssetRegistry",        // For asset handling
                             // Optional: If you ever want to access project settings/plugins at runtime
            }
        );

        // Private dependencies
        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine"
            }
        );

        // No dynamically loaded modules for now
        DynamicallyLoadedModuleNames.AddRange(new string[] { });
    }
}