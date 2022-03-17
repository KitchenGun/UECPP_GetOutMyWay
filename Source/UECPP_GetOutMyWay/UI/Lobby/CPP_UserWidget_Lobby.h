#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CPP_UserWidget_Lobby.generated.h"

UCLASS()
class UECPP_GETOUTMYWAY_API UCPP_UserWidget_Lobby : public UUserWidget
{
	GENERATED_BODY()
public:
	UCPP_UserWidget_Lobby(const FObjectInitializer &ObjectInitializer);
protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void ServerBrowserClicked();
	UFUNCTION()
	void ShootingRangeButtonClicked();
	UFUNCTION()
	void ExitButtonClicked();	
private:
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_ShootingRange;//나중에 멀티 들어가면 바꿔야함
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Exit;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_ServerBrowser;
};
