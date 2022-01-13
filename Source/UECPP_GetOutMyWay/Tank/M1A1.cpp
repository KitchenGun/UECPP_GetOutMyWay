#include "M1A1.h"
//사용하는 헤더
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"



AM1A1::AM1A1()
{
	PrimaryActorTick.bCanEverTick = true;
	TankMesh = this->CreateDefaultSubobject<USkeletalMeshComponent>("TankMesh");
	
}

void AM1A1::BeginPlay()
{
	Super::BeginPlay();
	
}

void AM1A1::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AM1A1::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

