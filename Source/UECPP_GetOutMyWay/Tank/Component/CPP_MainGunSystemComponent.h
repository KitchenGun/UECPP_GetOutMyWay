#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CPP_MainGunSystemComponent.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UECPP_GETOUTMYWAY_API UCPP_MainGunSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCPP_MainGunSystemComponent();
	UFUNCTION()
	virtual void MainGunFire();
protected:
	virtual void BeginPlay() override;
private:
	void ReloadDone();
	
protected:	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category=Shell)
	class ACPP_Projectile* Projectile;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category=Shell)
	TArray<int>Ammunition;
	//reload
	FTimerHandle ReloadTimerHandle;
	float ReloadTime = 1.0f;
	bool IsMainGunCanFire = true;
};
