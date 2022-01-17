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
	/*character movement ����*/
	//�̲��������� ����
	GetCharacterMovement()->GroundFriction = 1;				//������� �������� ����
	GetCharacterMovement()->BrakingDecelerationWalking = 2000;//���� Ŭ���� ���ӷ��� ������
	GetCharacterMovement()->Mass = 57000.0f;
}

void ACPP_M1A1::BeginPlay()
{
	Super::BeginPlay();
	//�ӵ� ������ ���� ���� �ʱ�ȭ
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
	{//������ ��� ����
		pitch=FMath::ClampAngle(pitch, PitchLimitMin, PitchLimitMax);
		
		//������ �´� ���� �־��ش�
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
		//������ ������ �����ϱ� ���ؼ�
		if (!IsMoveForward)
			value *= -1;
		//�������� �ƴ϶�� ������ �����ϰ� ȸ���������� ���ݾ� �̵��ϵ��� ����
		if(!IsAccelerating)
			AddMovementInput(GetMesh()->GetForwardVector(), 500 * GetWorld()->DeltaTimeSeconds);
		FRotator temp =FRotator(GetMesh()->GetRelativeRotation().Pitch, GetMesh()->GetRelativeRotation().Yaw + value, GetMesh()->GetRelativeRotation().Roll);
		GetMesh()->SetRelativeRotation(temp);


		//��ȸ �Ұ�� ��� ����
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
}

void ACPP_M1A1::OnEngineBreak()
{
	IsAccelerating = false;
}

void ACPP_M1A1::EngineControl()
{
	//�ӵ�
	CurrentVelocity = GetVelocity().Size();//m/s
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
	GetCharacterMovement()->MaxWalkSpeed = (RPM*EngineTorque) / ((10 - EngineGear)* 100);
	
	/*��¿�*/
	UE_LOG(LogTemp, Display, L"%.2f MaxWalkSpeed", GetCharacterMovement()->MaxWalkSpeed);//max 1250
	UE_LOG(LogTemp, Display, L"%.2f km/h", CurrentVelocity);								//max 75
	UE_LOG(LogTemp, Display, L"%.2f EngineTorque", EngineTorque);
	UE_LOG(LogTemp, Display, L"%.2f RPM", RPM);
	UE_LOG(LogTemp, Display, L"%d gear", EngineGear);
}

void ACPP_M1A1::RPMControl()
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


