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
		TurretAngleOffSet=Owner->GetActorRotation().Quaternion().Rotator().Yaw;
	}
	NextLocation = FVector::ZeroVector;
	NextRotation = FRotator::ZeroRotator;
}

void UCPP_TankPawnMovementComponent::OnMove(float value)
{
	TankClimbingAngle = Owner->GetActorRotation().Pitch;
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
		if(!FMath::IsNearlyEqual(VirtualForwardVal,1))
			VirtualForwardVal=FMath::Clamp(VirtualForwardVal+0.01f,0.0f,1.0f);
	}
	else if (FMath::IsNearlyZero(value))
	{
		IsAccelerating = false;
		IsMoveForward = true;


		if(VirtualForwardVal>0)
		{
			VirtualForwardVal=FMath::Clamp(VirtualForwardVal-VirtualFriction,0.0f,1.0f);
		}
		else if(VirtualForwardVal<0)
		{
			VirtualForwardVal=FMath::Clamp(VirtualForwardVal+VirtualFriction,-1.0f,0.0f);
		}
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
		if(!FMath::IsNearlyEqual(VirtualForwardVal,-1))
			VirtualForwardVal=FMath::Clamp(VirtualForwardVal-0.01f,-1.0f,0.0f);
	}

	//���ǰ��� ���� �ӵ� ����
	float TankClimbingAnglePercentage = 0.0f;
	if(TankClimbingAngle>0)
	{//�ö󰡴� ��Ȳ
		TankClimbingAnglePercentage=TankClimbingAngle/60;
	}
	else if(TankClimbingAngle<0)
	{//�������� ��Ȳ
		TankClimbingAnglePercentage=TankClimbingAngle/60;
	}
	else
	{
		TankClimbingAnglePercentage=0;
	}
	SetWheelSpeed(CurrentVelocity*(VirtualForwardVal-TankClimbingAnglePercentage));
	NextLocation+=(dir*(VirtualForwardVal-TankClimbingAnglePercentage));
	CurrentVelocity=(NextLocation*Speed*0.036f).Size();
	UE_LOG(LogTemp,Display,L"%.2f",CurrentVelocity);
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
		IsTurning = true;
	}
	else if (FMath::IsNearlyZero(value))
	{
		 IsTurning = false;
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
		if(IsTurretAngleMatch)
		{
			if(TurretMoveStartFunc.IsBound())
				TurretMoveStartFunc.Execute();			
		}
		
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
		if(!IsTurretAngleMatch)
		{
			if(TurretMoveEndFunc.IsBound())
				TurretMoveEndFunc.Execute();			
		}
		IsTurretAngleMatch = true;
	}
	TurretMove(DeltaTime);
}

void UCPP_TankPawnMovementComponent::TurretMove(float DeltaTime)
{
	if(!IsTurretAngleMatch)
	{
		//��ž �������� �þ� �������� ���� ���� ȸ���� ��
		float LeftAngel=0;
		float RightAngle=0;
		FixErrorRotator(TurretAngle);
		if(IsSightRight)
		{//sight�� ����
			if(IsTurretRight)
			{//TurretRotator.Yaw +�� ���
				if(TurretAngle<SightRotator.Yaw)
				{//�þ߰��� �� ū ���
					TurretAngle=TurretAngle+(DeltaTime*TurretTurnSpeed);
					if(TurretAngle>SightRotator.Yaw)
					{//�Ѿ ���
						TurretAngle = SightRotator.Yaw;
					}
				}
				else
				{//�þ߰��� �� ���� ���
					TurretAngle=TurretAngle-(DeltaTime*TurretTurnSpeed);
					if(TurretAngle<SightRotator.Yaw)
					{//�Ѿ ���
						TurretAngle = SightRotator.Yaw;
					}
				}
			}
			else
			{//TurretRotator.Yaw -�� ���
				LeftAngel = (180-abs(TurretAngle))+(180-SightRotator.Yaw);
				RightAngle = abs(TurretAngle)+SightRotator.Yaw;
				if(LeftAngel>RightAngle)
				{//���������� ȸ���ϴ� ���
					TurretAngle=TurretAngle+(DeltaTime*TurretTurnSpeed);
				}
				else
				{//�������� ȸ���ϴ� ���
					TurretAngle=TurretAngle-(DeltaTime*TurretTurnSpeed);
				}
				
			}
		}
		else
		{//sight�� ����
			if(IsTurretRight)
			{//TurretRotator.Yaw +�� ���
				LeftAngel = abs(SightRotator.Yaw)+TurretAngle;
				RightAngle = (180-abs(SightRotator.Yaw))+(180-TurretAngle);
				if(LeftAngel>RightAngle)
				{//���������� ȸ���ϴ� ���
					TurretAngle=TurretAngle+(DeltaTime*TurretTurnSpeed);
				}
				else
				{//�������� ȸ���ϴ� ���
					TurretAngle=TurretAngle-(DeltaTime*TurretTurnSpeed);
					FixErrorRotator(TurretAngle);
				}
			}
			else
			{//TurretRotator.Yaw -�� ���
				if(TurretAngle<SightRotator.Yaw)
				{//�þ߰��� �� ū ���
					TurretAngle=TurretAngle+(DeltaTime*TurretTurnSpeed);
					if(TurretAngle>SightRotator.Yaw)
					{//�Ѿ ���
						TurretAngle = SightRotator.Yaw;
					}
				}
				else
				{//�þ߰��� �� ���� ���
					TurretAngle=TurretAngle-(DeltaTime*TurretTurnSpeed);
					if(TurretAngle<SightRotator.Yaw)
					{//�Ѿ ���
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
		float TurretLocalRotationYaw = TankMesh->GetBoneQuaternion(L"turret_jnt",EBoneSpaces::ComponentSpace).Rotator().Yaw;
		//130 -> -10 150 ->0
		if(TurretLocalRotationYaw>0)
		{
			if(TurretLocalRotationYaw>130)
			{
				if(TurretLocalRotationYaw>150)
				{//150~max
					GunMinElevation = 0.0f;
				}
				else
				{//130~150
					float value = (TurretLocalRotationYaw - 130)/20;
					GunMinElevation=FMath::Lerp(-10,0,value);
				}
			}
			else
			{
				GunMinElevation = -10.0f;
			}
		}
		else
		{
			if(TurretLocalRotationYaw<-130)
			{
				if(TurretLocalRotationYaw<-150)
				{//-150~max
					GunMinElevation = 0.0f;
				}
				else
				{//-130~-150
					float value = -(TurretLocalRotationYaw + 130)/20;
					GunMinElevation=FMath::Lerp(-10,0,value);
				}
			}
			else
			{
				GunMinElevation = -10.0f;
			}
		}
		if(SightRotator.Pitch>GunAngle)
		{
			if(GunAngle+DeltaTime*GunMoveSpeed<FMath::ClampAngle(SightRotator.Pitch,GunMinElevation,GunMaxElevation))
				GunAngle = GunAngle+DeltaTime*GunMoveSpeed;
			else
				GunAngle = FMath::ClampAngle(GunAngle,GunMinElevation,GunMaxElevation);
		}
		else
		{
			if(GunAngle-DeltaTime*GunMoveSpeed>FMath::ClampAngle(SightRotator.Pitch,GunMinElevation,GunMaxElevation))
				GunAngle = FMath::ClampAngle(GunAngle-DeltaTime*GunMoveSpeed,GunMinElevation,GunMaxElevation);
			else
				GunAngle = FMath::ClampAngle(GunAngle,GunMinElevation,GunMaxElevation);
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
