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

	void OnMoveForward(float value);
	void OnMoveTurn(float value);
	void OnEngineBreak();

private:
	
	float CamRange = 800;
	float BasicCamTurnSpeed = 100;

	//Engine
	bool IsForward = true;
	float EngineTorque = 0.0f;
	int EngineGear = 0;
	float RPM = 500;
	float IdleRPM = 500;
	float MaxRPM = 6000;
	bool IsAccelerating = false;
	float AccelerationFactor = 6000.0f;
	FVector CurrentVelocity = FVector::ZeroVector;
	class UCurveFloat* EngineTorqueCurve;


};
