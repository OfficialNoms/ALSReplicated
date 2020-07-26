

using UnrealBuildTool;
using System.Collections.Generic;

public class ALSReplicatedTarget : TargetRules
{
	public ALSReplicatedTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "ALSReplicated" } );
	}
}
