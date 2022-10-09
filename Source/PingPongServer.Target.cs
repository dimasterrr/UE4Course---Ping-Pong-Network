using UnrealBuildTool;
using System.Collections.Generic;

public class PingPongServerTarget : TargetRules
{
	public PingPongServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange(new string[] {"PingPong"});
	}
}