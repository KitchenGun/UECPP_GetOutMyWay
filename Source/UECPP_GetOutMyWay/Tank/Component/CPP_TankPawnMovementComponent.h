#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "CPP_TankPawnMovementComponent.generated.h"

DECLARE_DYNAMIC_DELEGATE(FTurretMoveStart);
DECLARE_DYNAMIC_DELEGATE(FTurretMoveEnd);

UCLASS()
class UECPP_GETOUTMYWAY_API UCPP_TankPawnMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()
public:
	UCPP_TankPawnMovementComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//ABP에 전달할 변수 설정 함수
	void SetWheelSpeed(float WheelSpeed);
	void SetTurretAngleDetail();
	//이동
	void Movement(float DeltaTime);
	void OnMove(float value);
	void OnTurn(float value);
	
	void OnEngineBreak();
	void OffEngineBreak();

	
	//get&set
	FORCEINLINE float GetTrackSpeed() { return TrackSpeed; }
	FORCEINLINE float GetTurretAngle() { return TurretAngle; }
	FORCEINLINE float GetTurretAngleOffset() { return TurretAngleOffSet; }
	FORCEINLINE float GetGunAngle() {return GunAngle;}
	FORCEINLINE float GetGunAngleOffset() { return GunAngleOffSet; }
	FORCEINLINE bool GetIsMove() {return IsAccelerating||IsTurning;}
	FORCEINLINE void FixRotatorDirSize()
	{
		if(SightRotator.Yaw>TurretRotator.Yaw)
		{//포탑기준 오른쪽보는중
			LeftAngle = TurretRotator.Yaw+(360.0f-SightRotator.Yaw);
			RightAngle = SightRotator.Yaw - TurretRotator.Yaw;
		}
		else
		{//포탑기준 왼쪽보는중
			LeftAngle = TurretRotator.Yaw - SightRotator.Yaw;
			RightAngle = SightRotator.Yaw+(360.0f-TurretRotator.Yaw);
		}
	}

public:
	FTurretMoveStart TurretMoveStartFunc;
	FTurretMoveEnd TurretMoveEndFunc;
private:
	void EngineControl();
	void RPMControl();
	
	void UpdateTurretState(float DeltaTime);
	void TurretMove(float DeltaTime);
	void UpdateGunState(float DeltaTime);
	void GunMove(float DeltaTime);
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
	float VirtualForwardVal=0;
	float VirtualFriction = 0.01f;
	float TankClimbingAngle =0;
	//TrackSpeed
	float TrackSpeed = 0;

	//Engine 변수
	bool IsMoveForward = true;
	float TurnValue =0;
	float EngineTorque = 0.0f;
	int EngineGear = 0;
	float RPM = 500;
	bool IsAccelerating = false;
	bool IsTurning = false;
	float CurrentVelocity = 0;//
	float Speed = 100;
	bool isBreak = false;

	//Engine 객체 별로 수정할 데이터변수
	float TurnSpeed = 35;//선회 속도
	class UCurveFloat* EngineTorqueCurve;
	int MaxEngineGear = 4;
	float IdleRPM = 200;
	float RPMDisplacement = 200;
	float MinRPM = 200;
	float MaxRPM = 900;

	//Turret
	FRotator SightRotator =  FRotator::ZeroRotator;
	FRotator TurretRotator = FRotator::ZeroRotator; //world로 연산하도록 해야함
	bool IsTurretAngleMatch = true;
	bool IsSightRight = false; //차체 기준으로 오른쪽인가 왼쪽인가
	bool IsTurretRight = false; //차체 기준으로 오른쪽인가 왼쪽인가
	FVector SightDir = FVector::ZeroVector;
	FVector TurretDir = FVector::ZeroVector;
	float TurretAngle = 0.0f;
	float TurretAngleOffSet=0.0f;
	float LeftAngle=0;
	float RightAngle=0;
	//Turret 객체 별로 수정해야할 데이터 변수
	float TurretTurnSpeed = 100.0f;

	//Gun
	bool IsGunAngleMatch = true;
	FRotator GunRotator = FRotator::ZeroRotator;
	bool IsGunUpZero=true;
	bool IsSightUpZero=true;
	float GunAngle=0.0f;
	float GunAngleOffSet=0.0f;
	
	//Gun 객체 별로 수정해야할 데이터 변수
	float GunMoveSpeed = 50.0f;
	
};
