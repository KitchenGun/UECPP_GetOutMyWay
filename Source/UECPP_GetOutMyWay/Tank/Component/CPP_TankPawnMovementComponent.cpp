#include "Tank/Component/CPP_TankPawnMovementComponent.h"
#include "Tank/CPP_TankAnimInstance.h"
#include "GameFramework/Actor.h"
#include "Animation/AnimInstance.h"


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

void UCPP_TankPawnMovementComponent::SetWheelSpeed()
{
	if (!NextRotation.IsNearlyZero()&&NextLocation.IsNearlyZero())//방향전환 o, 이동 x
	{
		TrackSpeed = TurnSpeed;
	}
	else if (NextRotation.IsNearlyZero() && !NextLocation.IsNearlyZero())//방향전환 X, 이동 o
	{
		if(IsMoveForward)
			TrackSpeed=abs(TrackSpeed);
	}
}

void UCPP_TankPawnMovementComponent::Movement(float DeltaTime)
{
	if (Owner != nullptr && !NextLocation.IsNearlyZero())
	{
		NextLocation = GetActorLocation() + (NextLocation * DeltaTime * Speed);

		Owner->SetActorRelativeLocation(NextLocation);

		NextLocation = FVector::ZeroVector;
	}
	if (Owner != nullptr && !NextRotation.IsNearlyZero())
	{
		NextRotation = Owner->GetActorRotation() + (NextRotation * DeltaTime * TurnSpeed);

		Owner->SetActorRelativeRotation(NextRotation);

		NextRotation = FRotator::ZeroRotator;
	}
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
		//움직임에따라서 기어 맞추기
		//EngineGear = FMath::Clamp<int>(EngineGear, 0, MaxEngineGear);
		//기어에 맞게 RPM 조절
		if (IsMoveForward)
		{
			EngineGear = 0;
			RPMControl();
			RPM = FMath::Clamp<float>(RPM, MinRPM, MaxRPM);
			IsMoveForward = false;
		}
	}

	NextLocation+=(dir*value);
}

void UCPP_TankPawnMovementComponent::OnTurn(float value)
{
	//가속중이 아니라면 실제와 유사하게 회전방향으로 조금씩 이동하도록 만듬
	if (!FMath::IsNearlyZero(value))
	{
		//후진시 방향을 반전하기 위해서
		if (!IsMoveForward)
			value *= -1;
		//선회 할경우 기어 감소
		RPM -= (300 * GetWorld()->DeltaTimeSeconds);
		if (RPM <= MinRPM)
		{
			//Rpm이 낮아지면 기어를 낮춤
			if (EngineGear - 1 >= 0)
				EngineGear--;
		}
		if (RPM <= IdleRPM)
		{
			RPM = IdleRPM;
		}
	}

	NextRotation.Yaw+=value;
}

void UCPP_TankPawnMovementComponent::EngineControl()
{
	//속도
	CurrentVelocity = Owner->GetVelocity().Size();//m/s
	CurrentVelocity = (CurrentVelocity * 60) / 1000;//km/h 로 변환

	RPMControl();

	//엑셀레이션에 따른 RPM 변화량 적용
	if (IsAccelerating)
	{
		//가속시 RPM증가
		if (MaxRPM >= RPM)
		{
			RPM += (RPMDisplacement * GetWorld()->DeltaTimeSeconds);
			//UE_LOG(LogTemp, Display, L"%d gear", EngineGear);
			//UE_LOG(LogTemp, Display, L"%.2f MaxRPM", MaxRPM);
			//UE_LOG(LogTemp, Display, L"%.2f RPM", RPM);
		}
		else
		{
			//maxRpm 도달시 기어를 올림
			if (EngineGear + 1 <= MaxEngineGear)
				EngineGear++;
		}

	}
	else
	{
		//가속이 멈출경우 RPM감소
		RPM -= (300 * GetWorld()->DeltaTimeSeconds);
		if (RPM <= MinRPM)
		{
			//Rpm이 낮아지면 기어를 낮춤
			if (EngineGear - 1 >= 0)
				EngineGear--;
		}
		if (RPM <= IdleRPM)
		{
			RPM = IdleRPM;
		}
	}

	//구한 rpm 값으로 엔진토크 설정
	EngineTorque = EngineTorqueCurve->GetFloatValue(RPM);
	//단위 m/s
	Speed = (RPM * EngineTorque) / ((10 - EngineGear) * 100);

	/*출력용*/
	//UE_LOG(LogTemp, Display, L"%.2f MaxWalkSpeed", GetCharacterMovement()->MaxWalkSpeed);//max 1250
	//UE_LOG(LogTemp, Display, L"%.2f km/h", CurrentVelocity);								//max 75
	//UE_LOG(LogTemp, Display, L"%.2f EngineTorque", EngineTorque);
	//UE_LOG(LogTemp, Display, L"%.2f RPM", RPM);
	//UE_LOG(LogTemp, Display, L"%d gear", EngineGear);
}

void UCPP_TankPawnMovementComponent::RPMControl()
{
	//기어에 맞는 rpm 세팅
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
}
