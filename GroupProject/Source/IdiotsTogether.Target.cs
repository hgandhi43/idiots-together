using UnrealBuildTool;
using System.Collections.Generic;

public class IdiotsTogetherTarget : TargetRules
{
	public IdiotsTogetherTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		bOverrideBuildEnvironment = true;
		ExtraModuleNames.Add("IdiotsTogether");
	}
}
