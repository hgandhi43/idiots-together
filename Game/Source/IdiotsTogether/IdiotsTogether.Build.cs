using UnrealBuildTool;

public class IdiotsTogether : ModuleRules
{
	public IdiotsTogether(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"UMG",
			"Slate",
			"SlateCore",
			"NetCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });
	}
}
