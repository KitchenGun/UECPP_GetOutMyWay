#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CPP_ParticleControlComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UECPP_GETOUTMYWAY_API UCPP_ParticleControlComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCPP_ParticleControlComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
