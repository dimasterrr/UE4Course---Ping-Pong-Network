using UnrealBuildTool;

public class PingPong : ModuleRules
{
	public PingPong(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {"Core", "CoreUObject", "Engine", "InputCore", "UMG"});
		PrivateDependencyModuleNames.AddRange(new string[] {"OnlineSubsystem", "PakFile"});
	}
}