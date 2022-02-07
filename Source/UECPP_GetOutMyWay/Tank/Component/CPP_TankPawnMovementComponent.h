#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "CPP_TankPawnMovementComponent.generated.h"

UCLASS()
class UECPP_GETOUTMYWAY_API UCPP_TankPawnMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()
public:
	UCPP_TankPawnMovementComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//ABP�� ������ ���� ���� �Լ�
	void SetWheelSpeed();

	//�̵�
	void Movement(float DeltaTime);
	void OnMove(float value);
	void OnTurn(float value);
	void OnEngineBreak();
	void OffEngineBreak();

	//get&set
	FORCEINLINE float GetTrackSpeed() { return TrackSpeed; }
private:
	void EngineControl();
	void RPMControl();

private:
	class APawn* Owner;

	//mesh&ani
	class USkeletalMeshComponent* TankMesh;
	class UCPP_TankAnimInstance* TankAnimInst;

	//movement
	FVector NextLocation = FVector::ZeroVector;
	FRotator NextRotation = FRotator::ZeroRotator;
	FVector PrevPos = FVector::ZeroVector;
	float SpeedTimer = 0;
	//TrackSpeed
	float TrackSpeed = 0;

	//Engine ����
	bool IsMoveForward = true;
	float TurnValue =0;
	float EngineTorque = 0.0f;
	int EngineGear = 0;
	float RPM = 500;
	bool IsAccelerating = false;
	float CurrentVelocity = 0;//
	float Speed = 100;
	bool isBreak = false;

	//Engine ��ü ���� ������ �����ͺ���
	float TurnSpeed = 35;//��ȸ �ӵ�
	class UCurveFloat* EngineTorqueCurve;
	int MaxEngineGear = 4;
	float IdleRPM = 200;
	float RPMDisplacement = 200;
	float MinRPM = 200;
	float MaxRPM = 900;

	
};
