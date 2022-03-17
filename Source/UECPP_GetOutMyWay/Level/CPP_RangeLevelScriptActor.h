#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "CPP_RangeLevelScriptActor.generated.h"


UCLASS()
class UECPP_GETOUTMYWAY_API ACPP_RangeLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()
protected:
	
	virtual void BeginPlay() override;
};
