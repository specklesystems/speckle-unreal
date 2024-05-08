
using UnrealBuildTool;

public class SpeckleUnreal : ModuleRules
{
	public SpeckleUnreal(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		// #if UE_5_2_OR_LATER
		// IWYUSupport = IWYUSupport.Full;
		// #else 
		// bEnforceIWYU = true;
		// #endif
		//
		// bUseUnity = false;

		PublicDefinitions.Add("SPECKLE_CONNECTOR_VERSION=\"2.19.0\"");

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
				"HTTP", 
				"Json", 
				"JsonUtilities", 
				"ProceduralMeshComponent",
				"MeshDescription",
				"StaticMeshDescription", 
				"LidarPointCloudRuntime",
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
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
		);
	}
}
