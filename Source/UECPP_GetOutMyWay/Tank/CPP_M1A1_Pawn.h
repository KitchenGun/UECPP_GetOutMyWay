#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CPP_M1A1_Pawn.generated.h"


UENUM(BlueprintType)
enum class ECameraType : uint8
{
	THIRD		UMETA(DisplayName = "Third"),
	GUNNER		UMETA(DisplayName = "Gunner"),
	MAX
};

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
	class USceneComponent* GunnerCamPos;
	UPROPERTY(EditDefaultsOnly)
	class USpringArmComponent* SpringArm;

	//Collision
	UPROPERTY(VisibleDefaultsOnly)
	class UBoxComponent* FrontUpper;
	UPROPERTY(VisibleDefaultsOnly)
	class UBoxComponent* FrontUnder;
	UPROPERTY(VisibleDefaultsOnly)
	class UBoxComponent* Engine;
	UPROPERTY(VisibleDefaultsOnly)
	class UBoxComponent* Bottom;
	UPROPERTY(VisibleDefaultsOnly)
	class UBoxComponent* LSide;
	UPROPERTY(VisibleDefaultsOnly)
	class UBoxComponent* RSide;
	UPROPERTY(VisibleDefaultsOnly)
	class UBoxComponent* Turret;

	//ActorComp
	class UCPP_TrackMovementComponent* TrackMovement;

	class UCPP_TankPawnMovementComponent* TankMovement;

public:
	ACPP_M1A1_Pawn();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	//get&set
	//기존의 getmovementcomp 함수를 현재가지고 있는 함수로 교체하는 과정
	FORCEINLINE virtual UPawnMovementComponent* GetMovementComponent() const override;

private:
	void OnVerticalLook(float value);
	void OnHorizontalLook(float value);
	void CamPitchLimitSmooth();
	void CamChange();

	void OnMoveForward(float value);
	void OnMoveTurn(float value);
	void OnEngineBreak();
	void OffEngineBreak();
private:
	//sight
	float CamRange = 800;
	float BasicCamTurnSpeed = 100;
	float PitchLimitMax = 20;
	float PitchLimitMin = 270;
	ECameraType CamType = ECameraType::THIRD;

	//APlayerController
	APlayerController* PC = nullptr;

};
