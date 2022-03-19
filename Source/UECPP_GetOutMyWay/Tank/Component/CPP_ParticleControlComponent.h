#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Tank/CPP_Tank_Pawn.h"
#include "CPP_ParticleControlComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UECPP_GETOUTMYWAY_API UCPP_ParticleControlComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCPP_ParticleControlComponent();
	
	bool OnWheelParticle(bool IsMove);
	void OnFireParticle();
protected:
	virtual void BeginPlay() override;
	
private:
	class ACPP_Tank_Pawn* OwnerTank;
	//Particle
	class UParticleSystemComponent* MuzzleFlashEffect;
	class UParticleSystemComponent* ShockWaveEffect;
	TArray<class UParticleSystemComponent*> WheelsEffect;
};
