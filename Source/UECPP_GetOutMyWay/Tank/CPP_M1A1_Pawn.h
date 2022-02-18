#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CPP_M1A1_Pawn.generated.h"

DECLARE_DELEGATE(FFire);

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
	class UCameraComponent* GunnerCam;
	UPROPERTY(EditDefaultsOnly)
	class USpringArmComponent* GunnerSpringArm;
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

	class UCPP_M1A1MainGunSystemComponent* GunSystem;
public:
	ACPP_M1A1_Pawn();

	FFire FireFunc;
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	//get&set
	//������ getmovementcomp �Լ��� ���簡���� �ִ� �Լ��� ��ü�ϴ� ����
	FORCEINLINE virtual UPawnMovementComponent* GetMovementComponent() const override;
	
private:
	//sight
	void OnVerticalLook(float value);
	void OnHorizontalLook(float value);
	void CamPitchLimitSmooth();
	void CamChange();
	//move
	void OnMoveForward(float value);
	void OnMoveTurn(float value);
	void OnEngineBreak();
	void OffEngineBreak();
	//action
	void OnMainGunFire();
	
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
