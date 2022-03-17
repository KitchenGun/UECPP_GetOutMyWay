#include "UI/Lobby/CPP_UserWidget_Lobby.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

UCPP_UserWidget_Lobby::UCPP_UserWidget_Lobby(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	
}

void UCPP_UserWidget_Lobby::NativeConstruct()
{
	Super::NativeConstruct();

	Button_ServerBrowser->OnClicked.AddDynamic(this,&UCPP_UserWidget_Lobby::ServerBrowserClicked);
	Button_ShootingRange->OnClicked.AddDynamic(this,&UCPP_UserWidget_Lobby::ShootingRangeButtonClicked);
	Button_Exit->OnClicked.AddDynamic(this,&UCPP_UserWidget_Lobby::ExitButtonClicked);

	//나중에 멀티가면 player index 수정 필요할듯?//UI 전용으로 변경
	UGameplayStatics::GetPlayerController(GetWorld(),0);
	GetOwningPlayer()->SetInputMode(FInputModeUIOnly());
}

void UCPP_UserWidget_Lobby::ServerBrowserClicked()
{
	
}

void UCPP_UserWidget_Lobby::ShootingRangeButtonClicked()
{
	UGameplayStatics::OpenLevel(this,"TestLevel");
}

void UCPP_UserWidget_Lobby::ExitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(
		GetWorld(),
		UGameplayStatics::GetPlayerController(GetWorld(),0),
		EQuitPreference::Quit,
		false);
}
