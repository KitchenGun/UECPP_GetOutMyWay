#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "CPP_LobbyLevelScriptActor.generated.h"

UCLASS()
class UECPP_GETOUTMYWAY_API ACPP_LobbyLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()
public:
	ACPP_LobbyLevelScriptActor();
protected:
	virtual void BeginPlay() override;
private:
	TSubclassOf<class UCPP_UserWidget_Lobby> BP_LobbyWidget;
};
