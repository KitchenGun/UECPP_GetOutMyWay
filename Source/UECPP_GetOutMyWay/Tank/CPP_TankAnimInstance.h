// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Component/CPP_TrackMovementComponent.h"
#include "CPP_TankAnimInstance.generated.h"

UCLASS()
class UECPP_GETOUTMYWAY_API UCPP_TankAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Suspension")
	TArray<FWheelLocationData> BogieWheelData;

	UPROPERTY(BlueprintReadOnly,EditAnywhere,Category ="Track")
	float TrackSpeed = 0;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Turret")
	float CurTurretAngle =0;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Turret")
	float CurTurretAngleOffset =0;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Gun")
	float CurGunAngle=0.0f;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Gun")
	float CurGunAngleOffset =0;
	UPROPERTY(BlueprintReadOnly,EditAnywhere,Category ="Mesh")
	class USkeletalMeshComponent* TankMeshComp;

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
private:
	class APawn* Owner;
	class UCPP_TrackMovementComponent* TrackComp;
	class UCPP_TankPawnMovementComponent* TankMovementComp;
};
