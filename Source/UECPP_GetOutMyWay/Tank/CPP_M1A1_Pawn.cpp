#include "Tank/CPP_M1A1_Pawn.h"
//mesh
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Animation/AnimInstance.h"
//camera
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
//actorComp
#include "Component/CPP_TrackMovementComponent.h"
#include "Component/CPP_TankPawnMovementComponent.h"


ACPP_M1A1_Pawn::ACPP_M1A1_Pawn()
{
	PrimaryActorTick.bCanEverTick = true;
	/*객체 생성*/
	//mesh
	TankRoot = CreateDefaultSubobject<UStaticMeshComponent>(L"TankRoot");
	RootComponent = TankRoot;
	TankMesh = CreateDefaultSubobject<USkeletalMeshComponent>(L"TankMesh");
	TankMesh->SetupAttachment(TankRoot);
	//collider
		//차체
	FrontUpper = CreateDefaultSubobject<UBoxComponent>(L"FrontUpper");
	FrontUpper->SetupAttachment(TankRoot);
	Engine = CreateDefaultSubobject<UBoxComponent>(L"Engine");
	Engine->SetupAttachment(TankRoot);
	Bottom = CreateDefaultSubobject<UBoxComponent>(L"Bottom");
	Bottom->SetupAttachment(TankRoot);
	FrontUnder = CreateDefaultSubobject<UBoxComponent>(L"FrontUnder");
	FrontUnder->SetupAttachment(TankRoot);
	LSide = CreateDefaultSubobject<UBoxComponent>(L"LSide");
	LSide->SetupAttachment(TankRoot);
	RSide = CreateDefaultSubobject<UBoxComponent>(L"RSide");
	RSide->SetupAttachment(TankRoot);
		//포탑
	Turret = CreateDefaultSubobject<UBoxComponent>(L"Turret");
	Turret->SetupAttachment(TankRoot);

	//camera
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(L"SpringArm");
	SpringArm->SetupAttachment(TankMesh);
	Camera = CreateDefaultSubobject<UCameraComponent>(L"Camera");
	Camera->SetupAttachment(SpringArm);

	//actorcomp
	TrackMovement = CreateDefaultSubobject<UCPP_TrackMovementComponent>(L"TrackMovement");
	TankMovement = CreateDefaultSubobject<UCPP_TankPawnMovementComponent>(L"TankPawnMovement");
	/*객체 초기화*/
	//mesh
	ConstructorHelpers::FObjectFinder<UStaticMesh> smesh
	(L"StaticMesh'/Engine/BasicShapes/Cube.Cube'");
	TankRoot->SetStaticMesh(smesh.Object);
	TankRoot->SetSimulatePhysics(true);
	ConstructorHelpers::FObjectFinder<USkeletalMesh> skmesh
	(L"SkeletalMesh'/Game/VigilanteContent/Vehicles/West_Tank_M1A1Abrams/SK_West_Tank_M1A1Abrams.SK_West_Tank_M1A1Abrams'");
	TankMesh->SetSkeletalMesh(skmesh.Object);
	ConstructorHelpers::FClassFinder<UAnimInstance> animInst
	(L"AnimBlueprint'/Game/VigilanteContent/Vehicles/West_Tank_M1A1Abrams/ABP_West_Tank_M1A1Abrams.ABP_West_Tank_M1A1Abrams_C'");
	TankMesh->SetAnimInstanceClass(animInst.Class);
	TankMesh->SetRelativeLocation(FVector(0, 0, -100));
	//collider
		//bp에서 처리해야하나?
	FrontUpper->SetRelativeLocation(FVector(40,0,40));
	FrontUpper->SetRelativeRotation(FRotator(-5,0,0));
	FrontUpper->SetBoxExtent(FVector(350,170,10));
	FrontUpper->SetCollisionProfileName("PhysicsActor");
	Engine->SetRelativeLocation(FVector(-310, 0, 15));
	Engine->SetBoxExtent(FVector(32, 170, 60));
	Engine->SetCollisionProfileName("PhysicsActor");
	Bottom->SetRelativeLocation(FVector(-10, 0, -60));
	Bottom->SetBoxExtent(FVector(270, 170, 35));
	Bottom->SetCollisionProfileName("PhysicsActor");
	FrontUnder->SetRelativeLocation(FVector(300, 0, -35));
	FrontUnder->SetRelativeRotation(FRotator(25, 0, 0));
	FrontUnder->SetBoxExtent(FVector(60, 170, 25));
	FrontUnder->SetCollisionProfileName("PhysicsActor");
	LSide->SetRelativeLocation(FVector(-20, 130, 0));
	LSide->SetRelativeRotation(FRotator(-3, 0, 0));
	LSide->SetBoxExtent(FVector(330, 20, 60));
	LSide->SetCollisionProfileName("PhysicsActor");
	RSide->SetRelativeLocation(FVector(-20, -130, 0));
	RSide->SetRelativeRotation(FRotator(-3, 0, 0));
	RSide->SetBoxExtent(FVector(330, 20, 60));
	RSide->SetCollisionProfileName("PhysicsActor");
	Turret->SetRelativeLocation(FVector(0,0,110));
	Turret->SetBoxExtent(FVector(200,160,50));
	Turret->SetCollisionProfileName("PhysicsActor");
	//camera
	SpringArm->SetRelativeLocation(FVector(0, 0, 260));
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->TargetArmLength = CamRange;

}

void ACPP_M1A1_Pawn::BeginPlay()
{
	Super::BeginPlay();
}

void ACPP_M1A1_Pawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CamPitchLimitSmooth();
}

void ACPP_M1A1_Pawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("VerticalLook", this, &ACPP_M1A1_Pawn::OnVerticalLook);
	PlayerInputComponent->BindAxis("HorizontalLook", this, &ACPP_M1A1_Pawn::OnHorizontalLook);
	PlayerInputComponent->BindAxis("MoveForward", this, &ACPP_M1A1_Pawn::OnMoveForward);
	PlayerInputComponent->BindAxis("MoveTurn", this, &ACPP_M1A1_Pawn::OnMoveTurn);
	
	PlayerInputComponent->BindAction("EngineBreak",IE_Pressed,this, &ACPP_M1A1_Pawn::OnEngineBreak);
	PlayerInputComponent->BindAction("EngineBreak", IE_Released, this, &ACPP_M1A1_Pawn::OffEngineBreak);
}

UPawnMovementComponent* ACPP_M1A1_Pawn::GetMovementComponent() const
{
	return TankMovement;
}

void ACPP_M1A1_Pawn::OnVerticalLook(float value)
{
	AddControllerPitchInput(value * BasicCamTurnSpeed * GetWorld()->DeltaTimeSeconds);
}

void ACPP_M1A1_Pawn::OnHorizontalLook(float value)
{
	AddControllerYawInput(value * BasicCamTurnSpeed * GetWorld()->DeltaTimeSeconds);
}

void ACPP_M1A1_Pawn::CamPitchLimitSmooth()
{
	float pitch = Controller->GetControlRotation().Pitch;
	float limitVal = 0;
	//max를 수정해야함
	if (pitch < PitchLimitMin&&pitch>180)
	{//범위를 벗어난 상태
		pitch = FMath::ClampAngle(pitch, PitchLimitMin, PitchLimitMax);

		//범위에 맞는 값을 넣어준다
		FRotator temp = FRotator(pitch, GetControlRotation().Yaw, GetControlRotation().Roll);
		Controller->SetControlRotation(temp);
	}
	if(PitchLimitMax>180)
	{
		if (pitch < PitchLimitMax)
		{//범위를 벗어난 상태
			pitch = FMath::ClampAngle(pitch, PitchLimitMin, PitchLimitMax);

			//범위에 맞는 값을 넣어준다
			FRotator temp = FRotator(pitch, GetControlRotation().Yaw, GetControlRotation().Roll);
			Controller->SetControlRotation(temp);
		}
	}
	else
	{
		if (pitch < 180)
		{//범위를 벗어난 상태
			pitch = FMath::ClampAngle(pitch, 0, PitchLimitMax);

			//범위에 맞는 값을 넣어준다
			FRotator temp = FRotator(pitch, GetControlRotation().Yaw, GetControlRotation().Roll);
			Controller->SetControlRotation(temp);
		}
	}
}

void ACPP_M1A1_Pawn::OnMoveForward(float value)
{
	if (TankMovement != nullptr)
	{
		TankMovement->OnMove(value);
	}
}

void ACPP_M1A1_Pawn::OnMoveTurn(float value)
{
	if (TankMovement != nullptr)
	{
		TankMovement->OnTurn(value);
	}
}

void ACPP_M1A1_Pawn::OnEngineBreak()
{
	TankMovement->OnEngineBreak();
}

void ACPP_M1A1_Pawn::OffEngineBreak()
{
	TankMovement->OffEngineBreak();
}

