// Copyright 2017-2020 Puny Human, All Rights Reserved.

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
                "DeveloperSettings",
                "PhysicsCore",
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

        //Determine if we are using Chaos or PhysX, used for IgnoreBetween code
		if(Target.bUseChaos)
        {
            PrivateDependencyModuleNames.AddRange(
           new string[]
           {
                "Chaos",
           });
        }
		else
        {
            //Using Physx
            PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "PhysX",
                "APEX",
            });
        }

        //Determine if we have Modular Gameplay features
        if (true)
        {
            PublicDefinitions.Add("ARCVEHICLES_MODULAR=1");
            PrivateDependencyModuleNames.AddRange(
           new string[]
           {
                "ModularGameplay",
                "GameFeatures",
           });
        }
        else
        {
            PublicDefinitions.Add("ARCVEHICLES_MODULAR=0");
        }

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
