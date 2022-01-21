#include "Tank/CPP_TankAnimInstance.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Component/CPP_TrackMovementComponent.h"

void UCPP_TankAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Owner = TryGetPawnOwner();
}

void UCPP_TankAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if(Owner==nullptr)
		return;

	if(TrackComp==nullptr)
	{
		TrackComp = Cast<UCPP_TrackMovementComponent>
			(Owner->GetComponentByClass(UCPP_TrackMovementComponent::StaticClass()));
		BogieWheelData.SetNum(TrackComp->BogieWheelCount*2);
	}
	else
	{
		TrackComp->GetData(BogieWheelData);
	}
}