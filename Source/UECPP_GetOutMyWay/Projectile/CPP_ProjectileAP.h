#pragma once

#include "CoreMinimal.h"
#include "CPP_ProjectileDamageType.h"
#include "Projectile/CPP_Projectile.h"
#include "CPP_ProjectileAP.generated.h"

UCLASS()
class UECPP_GETOUTMYWAY_API ACPP_ProjectileAP : public ACPP_Projectile,public ICPP_ProjectileDamageType
{
	GENERATED_BODY()
public:
	UFUNCTION()
	virtual void BounceCal(float hitAngle, EHitDir hitDir) override;
protected:
	UFUNCTION()
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	
};
