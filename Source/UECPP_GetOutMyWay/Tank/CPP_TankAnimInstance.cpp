#include "Tank/CPP_TankAnimInstance.h"
#include "GameFramework/Pawn.h"
#include "Component/CPP_TrackMovementComponent.h"

void UCPP_TankAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Owner = TryGetPawnOwner();
	if (TrackComp == nullptr&& Owner != nullptr)
	{
		TrackComp = Cast<UCPP_TrackMovementComponent>
			(Owner->GetComponentByClass(UCPP_TrackMovementComponent::StaticClass()));
		if (TrackComp != nullptr)
			BogieWheelData.SetNum(TrackComp->BogieWheelCount * 2);
	}

	BogieWheelData=
	{
		FWheelLocationData(0,FVector::ZeroVector),
		FWheelLocationData(1,FVector::ZeroVector),
		FWheelLocationData(2,FVector::ZeroVector),
		FWheelLocationData(3,FVector::ZeroVector),
		FWheelLocationData(4,FVector::ZeroVector),
		FWheelLocationData(5,FVector::ZeroVector),
		FWheelLocationData(6,FVector::ZeroVector),
		FWheelLocationData(7,FVector::ZeroVector),
		FWheelLocationData(8,FVector::ZeroVector),
		FWheelLocationData(9,FVector::ZeroVector),
		FWheelLocationData(10,FVector::ZeroVector),
		FWheelLocationData(11,FVector::ZeroVector),
		FWheelLocationData(12,FVector::ZeroVector),
		FWheelLocationData(13,FVector::ZeroVector)
	};
}

void UCPP_TankAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if(Owner==nullptr)
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
}