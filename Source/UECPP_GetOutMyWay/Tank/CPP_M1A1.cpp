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
	//미끄러지도록 변경
	GetCharacterMovement()->GroundFriction = 1;				//지면과의 마찰강도 설정
	GetCharacterMovement()->BrakingDecelerationWalking = 350;//값이 클수록 감속량이 빨라짐
}

void ACPP_M1A1::BeginPlay()
{
	Super::BeginPlay();
}

void ACPP_M1A1::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CamPitchLimitSmooth();
	RPMControl();
}

void ACPP_M1A1::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("VerticalLook", this, &ACPP_M1A1::OnVerticalLook);
	PlayerInputComponent->BindAxis("HorizontalLook", this, &ACPP_M1A1::OnHorizontalLook);
	PlayerInputComponent->BindAxis("MoveForward", this, &ACPP_M1A1::OnMoveForward);
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
	//UE_LOG(LogTemp, Display, L"%.2f", GetVelocity().Size());
	//입력 값에 따른 엔진 관련 변수 초기화
	//엔진 출력은 torque*rpm
	//토크는 가속력을 의미한다
	if (value > 0)
	{
		IsMoveForward = true;
		IsAccelerating = true;
	}
	else if(FMath::IsNearlyZero(value))
	{
		IsAccelerating = false;
	}
	else
	{
		IsMoveForward = false;
		IsAccelerating = true;
	}

	if (IsAccelerating)
	{
		GetCharacterMovement()->MaxWalkSpeed = EngineTorque * (RPM*0.0005f);
		AddMovementInput(GetMesh()->GetForwardVector(), value*EngineTorque*GetWorld()->DeltaTimeSeconds);
	}
	
	auto temp = GetVelocity().Size();
	UE_LOG(LogTemp, Display, L"speed :: %.2f", temp);
	UE_LOG(LogTemp, Display, L"Torque:: %.2f", EngineTorque);
	UE_LOG(LogTemp, Display, L"RPM   :: %.2f", RPM);
}

void ACPP_M1A1::OnMoveTurn(float value)
{

}

void ACPP_M1A1::OnEngineBreak()
{
	IsAccelerating = false;
}

void ACPP_M1A1::RPMControl()
{
	if (BeforeIsMoveForward != IsMoveForward)
	{//전진과 후진에 따른 기어변속의 영향을 RPM이 받음
		RPM = (RPM - 700) >= IdleRPM ? RPM - 700 : IdleRPM*0.7f;
	}
	BeforeIsMoveForward = IsMoveForward;
	if (IsAccelerating)
	{
		//가속시 RPM증가
		if (MaxRPM > RPM)
		{
			RPM += (300 * GetWorld()->DeltaTimeSeconds);
		}
	}
	else
	{
		//가속이 멈출경우 RPM감소
		RPM -= (100 * GetWorld()->DeltaTimeSeconds);
		if (RPM <= IdleRPM)
		{
			RPM = 500;
		}
	}
	//구한 rpm 값으로 엔진토크 설정
	EngineTorque = EngineTorqueCurve->GetFloatValue(RPM);
}


