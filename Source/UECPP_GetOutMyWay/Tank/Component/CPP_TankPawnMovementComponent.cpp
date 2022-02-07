#include "Tank/Component/CPP_TankPawnMovementComponent.h"
#include "Tank/CPP_TankAnimInstance.h"
#include "GameFramework/Actor.h"
#include "Animation/AnimInstance.h"
#include "Media/Public/IMediaEventSink.h"


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
	EngineControl();
	Movement(DeltaTime);
}

void UCPP_TankPawnMovementComponent::SetWheelSpeed(float WheelSpeed)
{
	TrackSpeed = WheelSpeed*0.1f;
	//if(IsAccelerating)
	//	TrackSpeed = WheelSpeed;
	//else
	//	TrackSpeed=10*TurnValue;
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

void UCPP_TankPawnMovementComponent::OnEngineBreak()
{
	isBreak=true;
}

void UCPP_TankPawnMovementComponent::OffEngineBreak()
{
	isBreak = false;
}
