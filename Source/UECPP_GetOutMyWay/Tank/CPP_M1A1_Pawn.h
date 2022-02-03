#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CPP_M1A1_Pawn.generated.h"

UCLASS()
class UECPP_GETOUTMYWAY_API ACPP_M1A1_Pawn : public APawn
{
	GENERATED_BODY()
protected:
	//Root
	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* TankRoot;

	//Mesh
	UPROPERTY(EditDefaultsOnly)
	class USkeletalMeshComponent* TankMesh;

	//Camera
	UPROPERTY(EditDefaultsOnly)
	class UCameraComponent* Camera;
	UPROPERTY(EditDefaultsOnly)
	class USpringArmComponent* SpringArm;

	//Collision
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadWrite)
	class UBoxComponent* FrontUpper;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	class UBoxComponent* FrontUnder;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	class UBoxComponent* Engine;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	class UBoxComponent* Bottom;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	class UBoxComponent* LSide;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	class UBoxComponent* RSide;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	class UBoxComponent* Turret;

	//ActorComp
	UPROPERTY(EditDefaultsOnly)
	class UCPP_TrackMovementComponent* TrackMovement;

public:
	ACPP_M1A1_Pawn();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	

private:
	void OnVerticalLook(float value);
	void OnHorizontalLook(float value);
	void CamPitchLimitSmooth();

	void OnMoveForward(float value);
	void OnMoveTurn(float value);
private:
	//sight
	float CamRange = 800;
	float BasicCamTurnSpeed = 100;
	float PitchLimitMax = 360;
	float PitchLimitMin = 270;
};
