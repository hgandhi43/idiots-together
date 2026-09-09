using UnrealBuildTool;
using System.Collections.Generic;

public class GroupProjectTarget : TargetRules
{
	public GroupProjectTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		bOverrideBuildEnvironment = true;
		ExtraModuleNames.Add("GroupProject");
	}
}
