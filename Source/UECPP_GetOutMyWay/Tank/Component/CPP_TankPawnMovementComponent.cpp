#include "Tank/Component/CPP_TankPawnMovementComponent.h"
#include "GameFramework/Actor.h"

void UCPP_TankPawnMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	PrimaryComponentTick.bCanEverTick = true;
	Owner = Cast<APawn>(GetOwner());
}

void UCPP_TankPawnMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (Owner != nullptr && !NextLocation.IsNearlyZero())
	{
		NextLocation = GetActorLocation() + (NextLocation*DeltaTime*Speed);
		NextRotation = GetOwner()->GetActorRotation()+(NextRotation*DeltaTime*Speed);

		Owner->SetActorRelativeLocation(NextLocation);
		Owner->SetActorRelativeRotation(NextRotation);

		NextRotation = FRotator::ZeroRotator;
		NextLocation = FVector::ZeroVector;
	}

}

void UCPP_TankPawnMovementComponent::OnMove(float value)
{
	NextLocation.X+=value;
}

void UCPP_TankPawnMovementComponent::OnTurn(float value)
{
	NextRotation.Yaw+=value;
}
