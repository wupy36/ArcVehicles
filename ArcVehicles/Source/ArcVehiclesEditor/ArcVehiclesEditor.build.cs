// Copyright 2017-2020 Puny Human, All Rights Reserved.

using UnrealBuildTool;

public class ArcVehiclesEditor : ModuleRules
{
    public ArcVehiclesEditor(ReadOnlyTargetRules Target) : base(Target)
	{      

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "RenderCore",
            "AnimGraph",
            "BlueprintGraph",
            "ArcVehicles"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "Slate",
            "SlateCore",
            "InputCore",
            "UnrealEd",
            "RawMesh",
            "ContentBrowser",
            "AssetRegistry",
            "AssetTools",
            "LevelEditor",
            "EditorStyle",
            "PropertyEditor",
            "EditorWidgets",
            "RHI",
            "AssetTools"
        });

	}
}
