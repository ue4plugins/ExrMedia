// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class ExrMediaFactory : ModuleRules
	{
		public ExrMediaFactory(ReadOnlyTargetRules Target) : base(Target)
		{
            PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

            DynamicallyLoadedModuleNames.AddRange(
                new string[] {
                    "Media",
				}
            );

			PrivateDependencyModuleNames.AddRange(
				new string[] {
                    "MediaAssets",
                }
			);

			PrivateIncludePathModuleNames.AddRange(
				new string[] {
					"Media",
					"ExrMedia",
				}
			);

			PrivateIncludePaths.AddRange(
				new string[] {
					"ExrMediaFactory/Private",
				}
			);

            PublicDependencyModuleNames.AddRange(
                new string[] {
                    "Core",
                    "CoreUObject",
                }
            );

            if (Target.Type == TargetRules.TargetType.Editor)
			{
				DynamicallyLoadedModuleNames.Add("Settings");
				PrivateIncludePathModuleNames.Add("Settings");
			}

			if ((Target.Platform == UnrealTargetPlatform.Win32) ||
				(Target.Platform == UnrealTargetPlatform.Win64))
			{
				DynamicallyLoadedModuleNames.Add("ExrMedia");
			}
        }
	}
}
