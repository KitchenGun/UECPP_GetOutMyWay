#include "Level/CPP_LobbyLevelScriptActor.h"

#include "Kismet/GameplayStatics.h"
#include "UI/Lobby/CPP_UserWidget_Lobby.h"

ACPP_LobbyLevelScriptActor::ACPP_LobbyLevelScriptActor()
{
	ConstructorHelpers::FClassFinder<UCPP_UserWidget_Lobby> LobbyClass(L"WidgetBlueprint'/Game/BP/UI/Lobby/WB_Lobby.WB_Lobby_C'");
	BP_LobbyWidget = LobbyClass.Class;
}

void ACPP_LobbyLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();
	CreateWidget<UCPP_UserWidget_Lobby>(GetWorld(),BP_LobbyWidget)->AddToViewport();
	UGameplayStatics::GetPlayerController(GetWorld(),0)->SetInputMode(FInputModeUIOnly());
	UGameplayStatics::GetPlayerController(GetWorld(),0)->bShowMouseCursor = true;
}
