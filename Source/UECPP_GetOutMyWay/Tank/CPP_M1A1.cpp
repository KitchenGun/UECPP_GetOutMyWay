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
}

void ACPP_M1A1::BeginPlay()
{
	Super::BeginPlay();
}

void ACPP_M1A1::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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

void ACPP_M1A1::OnMoveForward(float value)
{
	//UE_LOG(LogTemp, Display, L"%.2f", GetVelocity().Size());
	//�Է� ���� ���� ���� ���� ���� �ʱ�ȭ
	//���� ����� torque*rpm
	//��ũ�� ���ӷ��� �ǹ��Ѵ�
	if (value > 0)
	{
		IsForward = true;
		IsAccelerating = true;
		GetCharacterMovement()->MaxWalkSpeed = 600;
	}
	else if(FMath::IsNearlyZero(value))
	{
		IsAccelerating = false;
		RPM = IdleRPM;
	}
	else
	{
		IsForward = false;
		IsAccelerating = true;
		GetCharacterMovement()->MaxWalkSpeed = 400;
	}


	if (IsAccelerating)
	{

		AddMovementInput(GetMesh()->GetForwardVector(), value*500*GetWorld()->DeltaTimeSeconds);
	}

}

void ACPP_M1A1::OnMoveTurn(float value)
{
}

void ACPP_M1A1::OnEngineBreak()
{
	IsAccelerating = false;

}


