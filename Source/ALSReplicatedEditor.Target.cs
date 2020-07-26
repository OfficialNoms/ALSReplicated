

using UnrealBuildTool;
using System.Collections.Generic;

public class ALSReplicatedEditorTarget : TargetRules
{
	public ALSReplicatedEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "ALSReplicated" } );
	}
}
