using UnrealBuildTool;
using System.Collections.Generic;

public class IdiotsTogetherEditorTarget : TargetRules
{
	public IdiotsTogetherEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		bOverrideBuildEnvironment = true;
		ExtraModuleNames.Add("IdiotsTogether");
	}
}
