using UnrealBuildTool;

public class SpeckleUnrealEditor : ModuleRules
{
	public SpeckleUnrealEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
		);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
		);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"Http", 
				"Json", 
				"JsonUtilities", 
				"ProceduralMeshComponent",
				"InputCore",
				"Slate",
				"SlateCore",
				"MainFrame",
				"EditorStyle"
			}
		);

		if (Target.bBuildEditor)
		{
			PublicDependencyModuleNames.AddRange(new string[]
			{
				"UnrealEd",
			});
		}
		
		if (Target.Type == TargetType.Editor) // Is UBT building for Editor ?
		{
			PrivateDependencyModuleNames.Add("SpeckleUnreal");
		}
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"Blutility",
				"Projects",
				"UMG",
				"UMGEditor"
				// ... add private dependencies that you statically link with here ...	
			}
		);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
		);
	}
}