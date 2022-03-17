#include "Level/CPP_RangeLevelScriptActor.h"
#include "Kismet/GameplayStatics.h"

void ACPP_RangeLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();
	UGameplayStatics::GetPlayerController(GetWorld(),0)->SetInputMode(FInputModeGameOnly());
	UGameplayStatics::GetPlayerController(GetWorld(),0)->bShowMouseCursor = false;
}
