#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CPP_M1A1.generated.h"


UCLASS()
class UECPP_GETOUTMYWAY_API ACPP_M1A1 : public ACharacter
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly)
		class UCameraComponent* Camera;
	UPROPERTY(EditDefaultsOnly)
		class USpringArmComponent* SpringArm;

public:
	ACPP_M1A1();

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
	void OnEngineBreak();
	void EngineControl();
	void RPMControl();
private:
	//sight
	float CamRange = 800;
	float BasicCamTurnSpeed = 100;
	float PitchLimitMax=360;
	float PitchLimitMin=270;
	
	//characterMovement
	float IdleGroundFriction = 8.0f;
	float IdleBrakingDecelerationWalking = 2048.0f;
	//speed
	FVector PrevPos = FVector::ZeroVector;
	FVector CurPos =FVector::ZeroVector;

	//Engine 변수
	bool IsMoveForward = true;
	float EngineTorque = 0.0f;
	int EngineGear = 0;
	float RPM = 500;
	bool IsAccelerating = false;
	float CurrentVelocity = 0;//

	//Engine 객체 별로 수정할 데이터변수
	float TurnSpeed = 45;
	class UCurveFloat* EngineTorqueCurve;
	int MaxEngineGear = 4;
	float IdleRPM = 200;
	float RPMDisplacement = 200;
	float MinRPM = 200;
	float MaxRPM = 900;


};
