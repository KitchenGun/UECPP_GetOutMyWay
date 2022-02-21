#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPP_Projectile.generated.h"

UCLASS()
class UECPP_GETOUTMYWAY_API ACPP_Projectile : public AActor
{
	GENERATED_BODY()
	
public:	
	ACPP_Projectile();

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
private:
	UPROPERTY(VisibleDefaultsOnly)
	class UCapsuleComponent* Capsule;
	UPROPERTY(VisibleDefaultsOnly)
	class UStaticMeshComponent* Shell;
	UPROPERTY(VisibleDefaultsOnly)
	class UStaticMeshComponent* WarHead;
	UPROPERTY(VisibleDefaultsOnly)
	class UStaticMeshComponent* Effect;
	
	class UProjectileMovementComponent* ProjectileMovement;
};
