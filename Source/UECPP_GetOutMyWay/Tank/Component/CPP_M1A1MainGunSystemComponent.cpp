
#include "Tank/Component/CPP_M1A1MainGunSystemComponent.h"
#include "Tank/CPP_M1A1_Pawn.h"

void UCPP_M1A1MainGunSystemComponent::BeginPlay()
{
	Super::BeginPlay();
	Owner=Cast<ACPP_M1A1_Pawn>(GetOwner());
	if(IsValid(Owner))
	{
		Owner->FireFunc.BindUFunction(this,"MainGunFire");
	}
}

UCPP_M1A1MainGunSystemComponent::UCPP_M1A1MainGunSystemComponent()
{
	ReloadTime =M1A1ReloadTime;
}

void UCPP_M1A1MainGunSystemComponent::MainGunFire()
{
	Super::MainGunFire();
	
}
