#include "Projectile/CPP_Projectile.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

ACPP_Projectile::ACPP_Projectile()
{
	//积己
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(L"Collider");
	Shell = CreateDefaultSubobject<UStaticMeshComponent>(L"Shell");
	WarHead = CreateDefaultSubobject<UStaticMeshComponent>(L"WarHead");
	Effect = CreateDefaultSubobject<UStaticMeshComponent>(L"Effect");
	MuzzleFlash = CreateDefaultSubobject<UParticleSystemComponent>(L"MuzzleFlash");
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(L"ProjectileMovement");

	//惑加备炼 沥府
	Shell->SetupAttachment(Capsule);
	WarHead->SetupAttachment(Capsule);
	Effect->SetupAttachment(Capsule);
	MuzzleFlash->SetupAttachment(Effect);
	//S*R*T
	Capsule->SetRelativeRotation(FRotator(90,0,0));
	Capsule->SetCapsuleHalfHeight(70);
	Capsule->SetCapsuleRadius(20);
	Shell->SetRelativeScale3D(FVector(10,10,10));
	Shell->SetRelativeRotation(FRotator(0,90,-90));
	Shell->SetRelativeLocation(FVector(0,0,20));
	WarHead->SetRelativeScale3D(FVector(0.05f,0.05f,0.05f));
	WarHead->SetRelativeRotation(FRotator(-90,0,0));
	WarHead->SetRelativeLocation(FVector(0,0,-20));
	Effect->SetRelativeScale3D(FVector(0.4f,0.1f,0.1f));
	Effect->SetRelativeRotation(FRotator(90,0,0));
	Effect->SetRelativeLocation(FVector(0,0,-45));
	MuzzleFlash->SetRelativeScale3D(FVector(2.5f,10.0f,10.0f));
	MuzzleFlash->SetRelativeRotation(FRotator(0,0,0));
	MuzzleFlash->SetRelativeLocation(FVector(250,0,0));
	//技何 汲沥
	ConstructorHelpers::FObjectFinder<UStaticMesh> shellMesh (L"StaticMesh'/Game/VigilanteContent/Shared/Particles/StaticMeshes/SM_BulletShell_01.SM_BulletShell_01'");
	Shell->SetStaticMesh(shellMesh.Object);
	ConstructorHelpers::FObjectFinder<UStaticMesh> warheadMesh(L"StaticMesh'/Game/VigilanteContent/Shared/Particles/StaticMeshes/SM_RocketBooster_02_SM.SM_RocketBooster_02_SM'");
	WarHead->SetStaticMesh(warheadMesh.Object);
	ConstructorHelpers::FObjectFinder<UMaterial> warheadMat(L"Material'/Game/VigilanteContent/Shared/Levels/Platform/LOGO/Materials/M_Vigilante_Logo.M_Vigilante_Logo'");
	WarHead->SetMaterial(0,warheadMat.Object);
	ConstructorHelpers::FObjectFinder<UStaticMesh> effectMesh(L"StaticMesh'/Game/VigilanteContent/Shared/Particles/StaticMeshes/SM_RocketBooster_03_SM.SM_RocketBooster_03_SM'");
	Effect->SetStaticMesh(effectMesh.Object);
	ConstructorHelpers::FObjectFinder<UParticleSystem> muzzleflashPS(L"ParticleSystem'/Game/VigilanteContent/Vehicles/West_Tank_M1A1Abrams/FX/PS_MuzzleFire_01_M1A1Abrams.PS_MuzzleFire_01_M1A1Abrams'");
	MuzzleFlash->SetTemplate(muzzleflashPS.Object);
	
	ProjectileMovement->InitialSpeed = 2e+4f;
	ProjectileMovement->MaxSpeed = 2e+4f;//20000
	ProjectileMovement->ProjectileGravityScale = 10;
}

void ACPP_Projectile::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACPP_Projectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	Destroy();
}


