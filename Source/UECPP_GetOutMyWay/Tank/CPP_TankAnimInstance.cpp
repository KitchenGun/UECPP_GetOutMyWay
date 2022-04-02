#include "Tank/CPP_TankAnimInstance.h"
#include "GameFramework/Pawn.h"
#include "Components/SkeletalMeshComponent.h"
#include "Component/CPP_TrackMovementComponent.h"
#include "Component/CPP_TankPawnMovementComponent.h"

void UCPP_TankAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Owner = TryGetPawnOwner();
	if(Owner!=nullptr)
	{
		TankMeshComp = Cast<USkeletalMeshComponent>(Owner->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	}
	
	
	if (TrackComp == nullptr&& Owner != nullptr)
	{
		TrackComp = Cast<UCPP_TrackMovementComponent>
			(Owner->GetComponentByClass(UCPP_TrackMovementComponent::StaticClass()));
		if (TrackComp != nullptr)
			BogieWheelData.SetNum(TrackComp->BogieWheelCount * 2);
	}
	if (TankMovementComp == nullptr && Owner != nullptr)
	{
		TankMovementComp = Cast<UCPP_TankPawnMovementComponent>
			(Owner->GetComponentByClass(UCPP_TankPawnMovementComponent::StaticClass()));
		if (TankMovementComp != nullptr)
		{
			TrackSpeed = TankMovementComp->GetTrackSpeed();
			
		}
	}

	BogieWheelData =
	{
		FWheelLocationData(0, FVector::ZeroVector),
		FWheelLocationData(1, FVector::ZeroVector),
		FWheelLocationData(2, FVector::ZeroVector),
		FWheelLocationData(3, FVector::ZeroVector),
		FWheelLocationData(4, FVector::ZeroVector),
		FWheelLocationData(5, FVector::ZeroVector),
		FWheelLocationData(6, FVector::ZeroVector),
		FWheelLocationData(7, FVector::ZeroVector),
		FWheelLocationData(8, FVector::ZeroVector),
		FWheelLocationData(9, FVector::ZeroVector),
		FWheelLocationData(10, FVector::ZeroVector),
		FWheelLocationData(11, FVector::ZeroVector),
		FWheelLocationData(12, FVector::ZeroVector),
		FWheelLocationData(13, FVector::ZeroVector)
	};
}

void UCPP_TankAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (Owner == nullptr)
		return;
	if (TrackComp == nullptr)
	{
		TrackComp = Cast<UCPP_TrackMovementComponent>
			(Owner->GetComponentByClass(UCPP_TrackMovementComponent::StaticClass()));
	}

	if (TrackComp != nullptr)
	{
		TrackComp->GetData(BogieWheelData);
	}

	if (TankMovementComp != nullptr)
	{
		TrackSpeed = TankMovementComp->GetTrackSpeed();
		CurTurretAngle = TankMovementComp->GetTurretAngle();
		
		CurTurretAngleOffset = TankMovementComp->GetTurretAngleOffset();
		CurGunAngle = TankMovementComp->GetGunAngle();
		CurGunAngleOffset = TankMovementComp->GetGunAngleOffset();
	}
}