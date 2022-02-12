#include "Tank/Component/CPP_TankPawnMovementComponent.h"
#include "Tank/CPP_TankAnimInstance.h"
#include "GameFramework/Actor.h"
#include "Animation/AnimInstance.h"
#include "Math/Vector.h"

UCPP_TankPawnMovementComponent::UCPP_TankPawnMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	Owner = Cast<APawn>(GetOwner());

	ConstructorHelpers::FObjectFinder<UCurveFloat> Curvefloat(L"CurveFloat'/Game/Data/Tank/Curve/FCurv_EngineTorque.FCurv_EngineTorque'");
	EngineTorqueCurve = Curvefloat.Object;


	if(Owner != nullptr)
		TankMesh = Cast<USkeletalMeshComponent>(Owner->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	if (TankMesh != nullptr)
	{
		TankAnimInst = Cast<UCPP_TankAnimInstance>(TankMesh->GetAnimInstance());
	}
}

void UCPP_TankPawnMovementComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCPP_TankPawnMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//Engine
	EngineControl();
	Movement(DeltaTime);
	//Turret
	UpdateTurretState(DeltaTime);
	//Gun
	UpdateGunState(DeltaTime);
}

void UCPP_TankPawnMovementComponent::SetWheelSpeed(float WheelSpeed)
{
	if(IsAccelerating)
	{
		TrackSpeed = WheelSpeed*0.1f;
	}
	else
	{
		TrackSpeed = TurnValue*2.0f;
	}
}

void UCPP_TankPawnMovementComponent::Movement(float DeltaTime)
{
	
	if (Owner != nullptr && !NextLocation.IsNearlyZero() && !isBreak)
	{
		NextLocation = GetActorLocation() + (NextLocation * DeltaTime * Speed);
		Owner->SetActorRelativeLocation(NextLocation);
	}
	if (Owner != nullptr && !NextRotation.IsNearlyZero())
	{
		NextRotation = Owner->GetActorRotation() + (NextRotation * DeltaTime * TurnSpeed);
		//������ ���ݾ� ������ �̵��ϵ��� ����
		if (!IsAccelerating)
		{
			NextLocation = GetActorLocation() + (Owner->GetActorForwardVector() * DeltaTime * 8.0f);
			Owner->SetActorRelativeLocation(NextLocation);
		}
		Owner->SetActorRelativeRotation(NextRotation);

	}
	NextLocation = FVector::ZeroVector;
	NextRotation = FRotator::ZeroRotator;
}

void UCPP_TankPawnMovementComponent::OnMove(float value)
{
	FVector dir = Owner->GetActorForwardVector();

	if (value > 0)
	{
		IsAccelerating = true;
		MaxEngineGear = 4;
		if (!IsMoveForward)
		{
			EngineGear = 0;
			RPMControl();
			RPM = FMath::Clamp<float>(RPM, MinRPM, MaxRPM);
			IsMoveForward = true;
		}
	}
	else if (FMath::IsNearlyZero(value))
	{
		IsAccelerating = false;
		IsMoveForward = true;
	}
	else
	{
		IsAccelerating = true;
		MaxEngineGear = 2;
		//�����ӿ����� ��� ���߱�
		//EngineGear = FMath::Clamp<int>(EngineGear, 0, MaxEngineGear);
		//�� �°� RPM ����
		if (IsMoveForward)
		{
			EngineGear = 0;
			RPMControl();
			RPM = FMath::Clamp<float>(RPM, MinRPM, MaxRPM);
			IsMoveForward = false;
		}
	}
	CurrentVelocity=(dir*Speed*0.036f).Size();
	
	SetWheelSpeed(CurrentVelocity*value);
	NextLocation+=(dir*value);
}

void UCPP_TankPawnMovementComponent::OnTurn(float value)
{
	//�������� �ƴ϶�� ������ �����ϰ� ȸ���������� ���ݾ� �̵��ϵ��� ����
	if (!FMath::IsNearlyZero(value))
	{
		//������ ������ �����ϱ� ���ؼ�
		if (!IsMoveForward)
			value *= -1;
		//��ȸ �Ұ�� ��� ����
		RPM -= (100 * GetWorld()->DeltaTimeSeconds);
		if (RPM <= MinRPM)
		{
			//Rpm�� �������� �� ����
			if (EngineGear - 1 >= 0)
				EngineGear--;
		}
		if (RPM <= IdleRPM)
		{
			RPM = IdleRPM;
		}
	}

	NextRotation.Yaw+=value;
	TurnValue = value;
}

void UCPP_TankPawnMovementComponent::EngineControl()
{
	//�ӵ�
	CurrentVelocity = Owner->GetVelocity().Size();//m/s
	CurrentVelocity = (CurrentVelocity * 60) / 1000;//km/h �� ��ȯ

	RPMControl();

	//�������̼ǿ� ���� RPM ��ȭ�� ����
	if (IsAccelerating)
	{
		//���ӽ� RPM����
		if (MaxRPM >= RPM)
		{
			RPM += (RPMDisplacement * GetWorld()->DeltaTimeSeconds);
			//UE_LOG(LogTemp, Display, L"%d gear", EngineGear);
			//UE_LOG(LogTemp, Display, L"%.2f MaxRPM", MaxRPM);
			//UE_LOG(LogTemp, Display, L"%.2f RPM", RPM);
		}
		else
		{
			//maxRpm ���޽� �� �ø�
			if (EngineGear + 1 <= MaxEngineGear)
				EngineGear++;
		}

	}
	else
	{
		//������ ������ RPM����
		RPM -= (300 * GetWorld()->DeltaTimeSeconds);
		if (RPM <= MinRPM)
		{
			//Rpm�� �������� �� ����
			if (EngineGear - 1 >= 0)
				EngineGear--;
		}
		if (RPM <= IdleRPM)
		{
			RPM = IdleRPM;
		}
	}

	//���� rpm ������ ������ũ ����
	EngineTorque = EngineTorqueCurve->GetFloatValue(RPM);
	//���� m/s
	Speed = (RPM * EngineTorque) / ((10 - EngineGear) * 100);

	/*��¿�*/
	//UE_LOG(LogTemp, Display, L"%.2f MaxWalkSpeed", GetCharacterMovement()->MaxWalkSpeed);//max 1250
	//UE_LOG(LogTemp, Display, L"%.2f km/h", CurrentVelocity);								//max 75
	//UE_LOG(LogTemp, Display, L"%.2f EngineTorque", EngineTorque);
	//UE_LOG(LogTemp, Display, L"%.2f RPM", RPM);
	//UE_LOG(LogTemp, Display, L"%d gear", EngineGear);
}

void UCPP_TankPawnMovementComponent::RPMControl()
{
	//�� �´� rpm ����
	switch (EngineGear)
	{
	case 0:
		MinRPM = 200;
		MaxRPM = 250;
		break;
	case 1:
		MinRPM = 200;
		MaxRPM = 600;
		break;
	case 2:
		MinRPM = 550;
		MaxRPM = 700;
		break;
	case 3:
		MinRPM = 650;
		MaxRPM = 800;
		break;
	case 4:
		MinRPM = 750;
		MaxRPM = 900;
		break;
	default:
		break;
	}
}

void UCPP_TankPawnMovementComponent::UpdateTurretState(float DeltaTime)
{
	SightRotator = Owner->GetController()->GetControlRotation().Quaternion().Rotator();
	TurretRotator = TankMesh->GetBoneQuaternion(L"turret_jnt").Rotator();
	
	if (!FMath::IsNearlyEqual(SightRotator.Yaw, TurretRotator.Yaw,0.1f))
	{
		//��ġ ���� ���� ���
		IsTurretAngleMatch = false;
		TurretDir = TankMesh->GetBoneQuaternion(L"turret_jnt").Vector();
		SightDir = SightRotator.Vector();
		//��ü�������� ���� ������ �˷��� �������̸� 0~180 �����̸� -180~0
		IsSightRight = GetIsRight(SightDir,FVector::ForwardVector);
		IsTurretRight = GetIsRight(TurretDir,FVector::ForwardVector);
	}
	else
	{
		IsTurretAngleMatch = true;
	}
	TurretMove(DeltaTime);
}

void UCPP_TankPawnMovementComponent::TurretMove(float DeltaTime)
{
	TurretAngleOffSet=Owner->GetActorRotation().Quaternion().Rotator().Yaw;
	if(!IsTurretAngleMatch)
	{
		//��ž �������� �þ� �������� ���� ���� ȸ���� ��
		float LeftAngel=0;
		float RightAngle=0;
		
		if(IsSightRight)
		{//sight�� ����
			if(IsTurretRight)
			{//turret�� ����
				if(TurretRotator.Yaw<SightRotator.Yaw)
				{
					//sight�� �� Ŭ ���
					if(TurretAngle+DeltaTime*TurretTurnSpeed<SightRotator.Yaw)
					{
						UE_LOG(LogTemp,Display,L"1");
						TurretAngle = FMath::ClampAngle(TurretAngle+DeltaTime*TurretTurnSpeed,0.0f,SightRotator.Yaw);
					}
					else
					{
						UE_LOG(LogTemp,Display,L"2");
						TurretAngle = SightRotator.Yaw;
					}
				}
				else
				{
					if(TurretAngle-DeltaTime*TurretTurnSpeed>SightRotator.Yaw)
					{
						UE_LOG(LogTemp,Display,L"3");
						TurretAngle = FMath::ClampAngle(TurretAngle-DeltaTime*TurretTurnSpeed,SightRotator.Yaw,180.0f);
					}
					else
					{
						UE_LOG(LogTemp,Display,L"4");
						TurretAngle = SightRotator.Yaw;
					}
				}
			}
			else
			{//TurretRotator.Yaw -�� ���
				LeftAngel = (180+TurretRotator.Yaw)+(180-SightRotator.Yaw);
				RightAngle = abs(TurretRotator.Yaw)+SightRotator.Yaw;
				if(RightAngle<LeftAngel)
				{
					//if(TurretAngle-DeltaTime*TurretTurnSpeed>SightRotator.Yaw)
					UE_LOG(LogTemp,Display,L"5");
						TurretAngle = TurretAngle+DeltaTime*TurretTurnSpeed;
					//else
					//	TurretAngle = SightRotator.Yaw;
				}
				else
				{
					UE_LOG(LogTemp,Display,L"6");
					//if(TurretAngle-DeltaTime*TurretTurnSpeed>SightRotator.Yaw)
						TurretAngle = TurretAngle-DeltaTime*TurretTurnSpeed;//FMath::ClampAngle(TurretAngle-DeltaTime*TurretTurnSpeed,-180,0.0f);
					//else
					//	TurretAngle = SightRotator.Yaw;
				}
			}
		}
		else
		{//sight�� ����
			if(IsTurretRight)
			{//turret�� ����
				LeftAngel = abs(SightRotator.Yaw)+TurretRotator.Yaw;
				RightAngle =(180+SightRotator.Yaw)+(180-TurretRotator.Yaw);
				if(RightAngle>LeftAngel)
				{
					UE_LOG(LogTemp,Display,L"7");
					TurretAngle = TurretAngle-DeltaTime*TurretTurnSpeed;
				}
				else
				{
					UE_LOG(LogTemp,Display,L"8");
					TurretAngle = TurretAngle+DeltaTime*TurretTurnSpeed;
				}
			}
			else
			{//TurretRotator.Yaw -�� ���
				if(TurretRotator.Yaw<SightRotator.Yaw)
				{ //sight�� �� Ŭ ���
					if(TurretAngle+DeltaTime*TurretTurnSpeed<SightRotator.Yaw)
					{
						UE_LOG(LogTemp,Display,L"9");
						TurretAngle = TurretAngle+DeltaTime*TurretTurnSpeed;//FMath::ClampAngle(TurretAngle+DeltaTime*TurretTurnSpeed,-180.0f,0);
					}
					else
					{
						UE_LOG(LogTemp,Display,L"10");
						TurretAngle = SightRotator.Yaw;
					}
				}
				else
				{
					if(TurretAngle-DeltaTime*TurretTurnSpeed>SightRotator.Yaw)
					{
						UE_LOG(LogTemp,Display,L"11");
						TurretAngle = TurretAngle-DeltaTime*TurretTurnSpeed;//FMath::ClampAngle(TurretAngle-DeltaTime*TurretTurnSpeed,-180,0.0f);
					}
					else
					{
						UE_LOG(LogTemp,Display,L"12");
						TurretAngle = SightRotator.Yaw;
					}
				}
			}
		}
	}
}

void UCPP_TankPawnMovementComponent::UpdateGunState(float DeltaTime)
{
	GunRotator = TankMesh->GetBoneQuaternion(L"gun_jnt").Rotator();
	if(!FMath::IsNearlyEqual(SightRotator.Pitch, GunRotator.Pitch,0.1f))
	{
		//��ġ ���� ���� ���
		IsGunAngleMatch = false;
		//GunAngle = GunRotator.Pitch;
		IsSightUpZero = SightRotator.Pitch>=0?true:false;
	}
	else
	{
		IsGunAngleMatch = true;
	}
	GunMove(DeltaTime);
}

void UCPP_TankPawnMovementComponent::GunMove(float DeltaTime)
{
	if(!IsGunAngleMatch)
	{
		if(SightRotator.Pitch>GunAngle)
		{
			if(GunAngle+DeltaTime*GunMoveSpeed<FMath::ClampAngle(SightRotator.Pitch,-10.0f,20.0f))
				GunAngle = GunAngle+DeltaTime*GunMoveSpeed;
		}
		else
		{
			if(GunAngle-DeltaTime*GunMoveSpeed>FMath::ClampAngle(SightRotator.Pitch,-10.0f,20.0f))
				GunAngle = GunAngle-DeltaTime*GunMoveSpeed;
		}
	}
}

void UCPP_TankPawnMovementComponent::OnEngineBreak()
{
	isBreak=true;
}

void UCPP_TankPawnMovementComponent::OffEngineBreak()
{
	isBreak = false;
}

bool UCPP_TankPawnMovementComponent::GetIsRight(FVector TargetVec, FVector ForwardVec)
{
	FVector CrossVec = FVector::CrossProduct(TargetVec, ForwardVec); //����
	if (FVector::DotProduct(CrossVec, FVector::UpVector) > 0) //����
		return false;
	else
		return true;
}
