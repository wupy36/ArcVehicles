// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ArcVehicles : ModuleRules
{
	public ArcVehicles(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.Default;
        PrivatePCHHeaderFile = "Public/ArcVehicles.h";


        PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
			);

        //Using Physx
        PrivateDependencyModuleNames.AddRange(
        new string[]
        {
            "PhysX",
            "APEX",
        });

		if (Target.Type == TargetType.Editor)
        {
            PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                    "Slate",
                    "SlateCore",
                    "UnrealEd",
            });
        }
    }
}
