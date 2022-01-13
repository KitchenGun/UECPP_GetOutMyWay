// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class UECPP_GetOutMyWayTarget : TargetRules
{
	public UECPP_GetOutMyWayTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "UECPP_GetOutMyWay" } );
	}
}
