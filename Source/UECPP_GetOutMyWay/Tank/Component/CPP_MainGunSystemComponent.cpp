#include "Tank/Component/CPP_MainGunSystemComponent.h"

UCPP_MainGunSystemComponent::UCPP_MainGunSystemComponent()
{
	
}

void UCPP_MainGunSystemComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UCPP_MainGunSystemComponent::ReloadDone()
{
	GetOwner()->GetWorldTimerManager().ClearTimer(ReloadTimerHandle);
	IsMainGunCanFire = true;
	UE_LOG(LogTemp,Display,L"UP!");
}

void UCPP_MainGunSystemComponent::MainGunFire()
{
	if(IsMainGunCanFire)
	{
		IsMainGunCanFire = false;
		UE_LOG(LogTemp,Display,L"fire");
		GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle,this,&UCPP_MainGunSystemComponent::ReloadDone,ReloadTime,false);
	}
}





