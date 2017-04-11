// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class OpenExrWrapper : ModuleRules
	{
		public OpenExrWrapper(ReadOnlyTargetRules Target) : base(Target)
		{
            bUseRTTI = true;
            PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

            PublicDependencyModuleNames.Add("Core");

            if ((Target.Platform == UnrealTargetPlatform.Win64) ||
                (Target.Platform == UnrealTargetPlatform.Win32) ||
                (Target.Platform == UnrealTargetPlatform.Mac))
            {
                AddEngineThirdPartyPrivateStaticDependencies(Target, "UEOpenExr");
                AddEngineThirdPartyPrivateStaticDependencies(Target, "zlib");
            }
            else
            {
                System.Console.WriteLine("OpenExrWrapper does not supported this platform");
            }
        }
	}
}
