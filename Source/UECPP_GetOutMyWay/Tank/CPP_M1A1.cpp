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
	//mesh
	ConstructorHelpers::FObjectFinder<USkeletalMesh> mesh
	(L"SkeletalMesh'/Game/VigilanteContent/Vehicles/West_Tank_M1A1Abrams/SK_West_Tank_M1A1Abrams.SK_West_Tank_M1A1Abrams'");
	GetMesh()->SetSkeletalMesh(mesh.Object);
	GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));
	//camera
	SpringArm->SetRelativeLocation(FVector(0, 0, 60));
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->TargetArmLength = CamRange;
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

}

