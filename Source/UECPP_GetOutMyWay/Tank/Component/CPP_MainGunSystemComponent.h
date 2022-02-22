#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CPP_MainGunSystemComponent.generated.h"

DECLARE_DELEGATE(FFireEffect)
DECLARE_DELEGATE(FReloadDone)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UECPP_GETOUTMYWAY_API UCPP_MainGunSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCPP_MainGunSystemComponent();
	UFUNCTION()
	virtual void MainGunFire();

	FFireEffect FireEffectFunc;
	FReloadDone GunReloadDoneFunc;
	
	FORCEINLINE bool GetIsMainGunCanFire() {return IsMainGunCanFire;}
protected:
	virtual void BeginPlay() override;
private:
	void ReloadDone();
	
protected:	
	UPROPERTY(EditDefaultsOnly, Category = "Shell")
	TSubclassOf<class ACPP_Projectile> ProjectileClass;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category=Shell)
	TArray<int32>Ammunition;
	//reload
	FTimerHandle ReloadTimerHandle;
	float ReloadTime = 1.0f;
	bool IsMainGunCanFire = true;
	//mesh
	class USkeletalMeshComponent* TankMesh;
};
