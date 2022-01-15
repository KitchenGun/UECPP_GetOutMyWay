#include "Tank/CPP_M1A1.h"
//����ϴ� ���
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
	/*��ü ����*/
	//camera
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(L"SpringArm");
	SpringArm->SetupAttachment(GetMesh());
	Camera = CreateDefaultSubobject<UCameraComponent>(L"Camera");
	Camera->SetupAttachment(SpringArm);

	/*��ü �ʱ�ȭ*/
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
	//�̲��������� ����
	GetCharacterMovement()->GroundFriction = 1;				//������� �������� ����
	GetCharacterMovement()->BrakingDecelerationWalking = 350;//���� Ŭ���� ���ӷ��� ������
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
	{//������ ��� ����
		pitch=FMath::ClampAngle(pitch, PitchLimitMin, PitchLimitMax);
		
		//������ �´� ���� �־��ش�
		FRotator temp = FRotator(pitch, GetControlRotation().Yaw, GetControlRotation().Roll);
		Controller->SetControlRotation(temp);
	}

}

void ACPP_M1A1::OnMoveForward(float value)
{
	//UE_LOG(LogTemp, Display, L"%.2f", GetVelocity().Size());
	//�Է� ���� ���� ���� ���� ���� �ʱ�ȭ
	//���� ����� torque*rpm
	//��ũ�� ���ӷ��� �ǹ��Ѵ�
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
	{//������ ������ ���� ������ ������ RPM�� ����
		RPM = (RPM - 700) >= IdleRPM ? RPM - 700 : IdleRPM*0.7f;
	}
	BeforeIsMoveForward = IsMoveForward;
	if (IsAccelerating)
	{
		//���ӽ� RPM����
		if (MaxRPM > RPM)
		{
			RPM += (300 * GetWorld()->DeltaTimeSeconds);
		}
	}
	else
	{
		//������ ������ RPM����
		RPM -= (100 * GetWorld()->DeltaTimeSeconds);
		if (RPM <= IdleRPM)
		{
			RPM = 500;
		}
	}
	//���� rpm ������ ������ũ ����
	EngineTorque = EngineTorqueCurve->GetFloatValue(RPM);
}


