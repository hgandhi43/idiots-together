using UnrealBuildTool;

public class GroupProject : ModuleRules
{
	public GroupProject(ReadOnlyTargetRules Target) : base(Target)
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
