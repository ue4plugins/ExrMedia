// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class ExrMedia : ModuleRules
	{
		public ExrMedia(TargetInfo Target)
		{
            DynamicallyLoadedModuleNames.AddRange(
				new string[] {
					"Media",
				}
			);

			PrivateDependencyModuleNames.AddRange(
				new string[] {
					"Core",
                    "CoreUObject",
                    "Engine",
                    "ExrMediaFactory",
                    "OpenExrWrapper",
					"RenderCore",
					"RHI",
				}
			);

			PrivateIncludePathModuleNames.AddRange(
				new string[] {
					"Media",
				}
			);

			PrivateIncludePaths.AddRange(
				new string[] {
					"ExrMedia/Private",
                    "ExrMedia/Private/Assets",
                    "ExrMedia/Private/Player",
				}
			);

            PublicDependencyModuleNames.AddRange(
                new string[] {
                    "MediaAssets",
                }
            );
        }
	}
}
