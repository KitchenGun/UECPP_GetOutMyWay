#include "Tank/CPP_M1A1.h"
//사용하는 헤더
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
//mesh
#include "Components/SkeletalMeshComponent.h"
//camera
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"


ACPP_M1A1::ACPP_M1A1()
{
	PrimaryActorTick.bCanEverTick = true;
	/*객체 생성*/
	//camera
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(L"SpringArm");
	SpringArm->SetupAttachment(GetMesh());
	Camera = CreateDefaultSubobject<UCameraComponent>(L"Camera");
	Camera->SetupAttachment(SpringArm);

	/*객체 초기화*/
	bUseControllerRotationYaw = false;
	//mesh
	ConstructorHelpers::FObjectFinder<USkeletalMesh> mesh
	(L"SkeletalMesh'/Game/VigilanteContent/Vehicles/West_Tank_M1A1Abrams/SK_West_Tank_M1A1Abrams.SK_West_Tank_M1A1Abrams'");
	GetMesh()->SetSkeletalMesh(mesh.Object);
	GetMesh()->SetRelativeLocation(FVector(0, 0, -120));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));
	//camera
	SpringArm->SetRelativeLocation(FVector(0, 0, 60));
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->TargetArmLength = CamRange;
	//engine
	ConstructorHelpers::FObjectFinder<UCurveFloat> Curvefloat(L"CurveFloat'/Game/Curve/EngineTorque.EngineTorque'");
	EngineTorqueCurve = Curvefloat.Object;
	//CharacterMovementComponent
	/*character movement 설정*/
	//미끄러지도록 변경
	GetCharacterMovement()->GroundFriction = 1;				//지면과의 마찰강도 설정
	GetCharacterMovement()->BrakingDecelerationWalking = 2000;//값이 클수록 감속량이 빨라짐
	GetCharacterMovement()->Mass = 57000.0f;
}

void ACPP_M1A1::BeginPlay()
{
	Super::BeginPlay();
	//속도 측정을 위한 변수 초기화
	CurPos = this->GetActorLocation();
	PrevPos = this->GetActorLocation();
}

void ACPP_M1A1::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CamPitchLimitSmooth();
	EngineControl();
}

void ACPP_M1A1::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("VerticalLook", this, &ACPP_M1A1::OnVerticalLook);
	PlayerInputComponent->BindAxis("HorizontalLook", this, &ACPP_M1A1::OnHorizontalLook);
	PlayerInputComponent->BindAxis("MoveForward", this, &ACPP_M1A1::OnMoveForward);
	PlayerInputComponent->BindAxis("MoveTurn", this, &ACPP_M1A1::OnMoveTurn);
}


void ACPP_M1A1::OnVerticalLook(float value)
{
	AddControllerPitchInput(value * BasicCamTurnSpeed * GetWorld()->DeltaTimeSeconds);
}

void ACPP_M1A1::OnHorizontalLook(float value)
{
	AddControllerYawInput(value * BasicCamTurnSpeed * GetWorld()->DeltaTimeSeconds);
}

void ACPP_M1A1::CamPitchLimitSmooth()
{
	float pitch = Controller->GetControlRotation().Pitch;
	float limitVal = 0;
	if (pitch < PitchLimitMin || PitchLimitMax < pitch)
	{//범위를 벗어난 상태
		pitch=FMath::ClampAngle(pitch, PitchLimitMin, PitchLimitMax);
		
		//범위에 맞는 값을 넣어준다
		FRotator temp = FRotator(pitch, GetControlRotation().Yaw, GetControlRotation().Roll);
		Controller->SetControlRotation(temp);
	}

}

void ACPP_M1A1::OnMoveForward(float value)
{

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
	else if(FMath::IsNearlyZero(value))
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

	if (IsAccelerating)
	{
		AddMovementInput(GetMesh()->GetForwardVector(), value*1000*GetWorld()->DeltaTimeSeconds);
	}
}

void ACPP_M1A1::OnMoveTurn(float value)
{
	if (!FMath::IsNearlyZero(value))
	{
		value = value * TurnSpeed * GetWorld()->DeltaTimeSeconds;
		//후진시 방향을 반전하기 위해서
		if (!IsMoveForward)
			value *= -1;
		//가속중이 아니라면 실제와 유사하게 회전방향으로 조금씩 이동하도록 만듬
		if(!IsAccelerating)
			AddMovementInput(GetMesh()->GetForwardVector(), 500 * GetWorld()->DeltaTimeSeconds);
		FRotator temp =FRotator(GetMesh()->GetRelativeRotation().Pitch, GetMesh()->GetRelativeRotation().Yaw + value, GetMesh()->GetRelativeRotation().Roll);
		GetMesh()->SetRelativeRotation(temp);


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
}

void ACPP_M1A1::OnEngineBreak()
{
	IsAccelerating = false;
}

void ACPP_M1A1::EngineControl()
{
	//속도
	CurrentVelocity = GetVelocity().Size();//m/s
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
	GetCharacterMovement()->MaxWalkSpeed = (RPM*EngineTorque) / ((10 - EngineGear)* 100);
	
	/*출력용*/
	UE_LOG(LogTemp, Display, L"%.2f MaxWalkSpeed", GetCharacterMovement()->MaxWalkSpeed);//max 1250
	UE_LOG(LogTemp, Display, L"%.2f km/h", CurrentVelocity);								//max 75
	UE_LOG(LogTemp, Display, L"%.2f EngineTorque", EngineTorque);
	UE_LOG(LogTemp, Display, L"%.2f RPM", RPM);
	UE_LOG(LogTemp, Display, L"%d gear", EngineGear);
}

void ACPP_M1A1::RPMControl()
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


