#include "Tank/Component/CPP_TankPawnMovementComponent.h"
#include "Tank/CPP_TankAnimInstance.h"
#include "GameFramework/Actor.h"
#include "Animation/AnimInstance.h"
#include "Math/Vector.h"
#include "Tank/CPP_Tank_Pawn.h"

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
	if (Owner != nullptr && !NextLocation.IsNearlyZero())
	{
		NextLocation = GetActorLocation() + (NextLocation * DeltaTime * Speed);
		Owner->SetActorRelativeLocation(NextLocation);
	}
	if (Owner != nullptr && !NextRotation.IsNearlyZero())
	{
		NextRotation = Owner->GetActorRotation() + (NextRotation * DeltaTime * TurnSpeed);
		//정지시 조금씩 앞으로 이동하도록 제작
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

	if (value > 0&&!isBreak)
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
	else if (FMath::IsNearlyZero(value)||isBreak)
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
	else if(!isBreak)
	{
		IsAccelerating = true;
		MaxEngineGear = 2;
		//기어에 맞게 RPM 조절
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

	//등판각에 따른 속도 조절
	float TankClimbingAnglePercentage = 0.0f;
	if(TankClimbingAngle>0)
	{//올라가는 상황
		TankClimbingAnglePercentage=TankClimbingAngle/60;
	}
	else if(TankClimbingAngle<0)
	{//내려가는 상황
		TankClimbingAnglePercentage=TankClimbingAngle/60;
	}
	else
	{
		TankClimbingAnglePercentage=0;
	}
	NextLocation+=(dir*(VirtualForwardVal-TankClimbingAnglePercentage));
	CurrentVelocity=(NextLocation*Speed*0.036f).Size();
	//애니메이션에 전달
	SetWheelSpeed(CurrentVelocity*VirtualForwardVal);
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
		RPM -= (100 * GetWorld()->DeltaTimeSeconds);
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

void UCPP_TankPawnMovementComponent::UpdateTurretState(float DeltaTime)
{
	SightRotator = Owner->GetController()->GetControlRotation().Quaternion().Rotator()+FRotator(0,180,0);
	TurretRotator = TankMesh->GetBoneQuaternion(L"turret_jnt").Rotator()+FRotator(0,180,0);
	if (!FMath::IsNearlyZero(SightRotator.Yaw-TurretRotator.Yaw,0.1f))
	{
		//포탑회전 관련 사운드 함수 호출
		if(IsTurretAngleMatch)
		{
			if(TurretMoveStartFunc.IsBound())
				TurretMoveStartFunc.Execute();			
		}
		
		//일치 하지 않을 경우
		IsTurretAngleMatch = false;
	}
	else
	{
		//포탑회전 관련 사운드 함수 호출
		if(!IsTurretAngleMatch)
		{
			if(TurretMoveEndFunc.IsBound())
				TurretMoveEndFunc.Execute();			
		}
		IsTurretAngleMatch = true;
	}
	if(!IsTurretAngleMatch)
		TurretMove(DeltaTime);
}

void UCPP_TankPawnMovementComponent::TurretMove(float DeltaTime)
{
	if(IsTurretAngleMatch)
		return;
	//탱크 포탑 기준  바라보는 곳과 몇도가 차이 나는지 판단용
	FixRotatorDirSize();
	//더 작은 각도 판별
	if(LeftAngle<RightAngle)
	{
		if(LeftAngle>DeltaTime*TurretTurnSpeed)
		{
			TurretAngle = TurretAngle-(DeltaTime*TurretTurnSpeed);//크기가 작은쪽으로 회전
		}
		else
		{//회전 방향의 남은 각도와 사용자가 바라보는 각도와 보간
			FRotator temp = FMath::RInterpTo(FRotator(0,TurretAngle,0),
				FRotator(0,SightRotator.Yaw,0)-FRotator(0,180,0),DeltaTime,TurretTurnSpeed);
			
			TurretAngle=temp.Yaw;
		}
	}
	else if(LeftAngle>RightAngle)
	{
		if(RightAngle>DeltaTime*TurretTurnSpeed)
		{
			TurretAngle = TurretAngle+(DeltaTime*TurretTurnSpeed);//크기가 작은쪽으로 회전
		}
		else
		{
			//회전 방향의 남은 각도와 사용자가 바라보는 각도와 보간
			FRotator temp = FMath::RInterpTo(FRotator(0,TurretAngle,0),
				FRotator(0,SightRotator.Yaw,0)-FRotator(0,180,0),DeltaTime,TurretTurnSpeed);
			TurretAngle=temp.Yaw;
		}
	}
}

void UCPP_TankPawnMovementComponent::UpdateGunState(float DeltaTime)
{
	SightRotator = Owner->GetController()->GetControlRotation().Quaternion().Rotator();
	GunRotator = TankMesh->GetBoneQuaternion(L"gun_jnt").Rotator().Quaternion().Rotator();
	if(!FMath::IsNearlyEqual(SightRotator.Pitch, GunRotator.Pitch,0.01f))
	{
		//일치 하지 않을 경우
		IsGunAngleMatch = false;
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
	if(IsGunAngleMatch)//매치하면 반환
		return;
	//시선을 등판각을 합쳐서 tankmesh의 gun rotator에 맞게 변경
	float targetPitch = SightRotator.Pitch-Cast<ACPP_Tank_Pawn>(Owner)->GetGunAngleOffset();
	//gun본의 rotator를 받아와옴
	float GunRotationPitch = TankMesh->GetBoneQuaternion(L"gun_jnt",EBoneSpaces::ComponentSpace).Rotator().Pitch;
	if(GunRotationPitch>targetPitch)
	{//목표로하는 각도가 gun의 각도보다 작은 경우
		GunRotationPitch = FMath::Clamp(GunRotationPitch-(GunMoveSpeed*DeltaTime),targetPitch,GunRotationPitch);
	}
	else if(GunRotationPitch<targetPitch)
	{//목표로하는 각도가 gun의 각도보다 큰 경우
		GunRotationPitch =FMath::Clamp(GunRotationPitch+(GunMoveSpeed*DeltaTime),GunRotationPitch,targetPitch);
	}
	//마지막으로 animinst에서 접근하는 변수에 넣어준다.
	GunAngle = GunRotationPitch;
}

void UCPP_TankPawnMovementComponent::OnEngineBreak()
{
	isBreak=true;
	VirtualFriction = 0.02f;
}

void UCPP_TankPawnMovementComponent::OffEngineBreak()
{
	isBreak = false;
	VirtualFriction = 0.01f;
}


