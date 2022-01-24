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
	class UBoxComponent* TankRoot;

	//Mesh
	UPROPERTY(EditDefaultsOnly)
	class USkeletalMeshComponent* TankMesh;

	//Camera
	UPROPERTY(EditDefaultsOnly)
	class UCameraComponent* Camera;
	UPROPERTY(EditDefaultsOnly)
	class USpringArmComponent* SpringArm;

	//Collision
	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* FrontUpper;
	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* FrontUnder;
	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* Engine;
	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* Bottom;

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

};
