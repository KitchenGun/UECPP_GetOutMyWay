#include "Tank/Component/CPP_ParticleControlComponent.h"

UCPP_ParticleControlComponent::UCPP_ParticleControlComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UCPP_ParticleControlComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


void UCPP_ParticleControlComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

