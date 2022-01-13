// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class UECPP_GetOutMyWayEditorTarget : TargetRules
{
	public UECPP_GetOutMyWayEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "UECPP_GetOutMyWay" } );
	}
}
