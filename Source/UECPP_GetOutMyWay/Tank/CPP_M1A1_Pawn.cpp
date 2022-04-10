#include "Tank/CPP_M1A1_Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
//mesh
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Animation/AnimInstance.h"
//camera
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
//sound
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Sound/SoundAttenuation.h"
//actorComp
#include "Component/CPP_TrackMovementComponent.h"
#include "Component/CPP_TankPawnMovementComponent.h"
#include "Component/CPP_M1A1MainGunSystemComponent.h"
#include "Component/CPP_ParticleControlComponent.h"
#include "Engine/AssetManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceConstant.h"


ACPP_M1A1_Pawn::ACPP_M1A1_Pawn()
{
	//parameter
	ParameterSet();
	//mesh
	RootSet();
	//collider
	CollisionSet();
	//camera
	CameraSet();
	//particle
	ParticleSet();
	//sound
	SoundSet();
	//actorcomp
	TrackMovement = CreateDefaultSubobject<UCPP_TrackMovementComponent>(L"TrackMovement");
	TankMovement = CreateDefaultSubobject<UCPP_TankPawnMovementComponent>(L"TankPawnMovement");
	GunSystem = CreateDefaultSubobject<UCPP_M1A1MainGunSystemComponent>(L"GunSystem");
	ParticleSystem = CreateDefaultSubobject<UCPP_ParticleControlComponent>(L"ParticleController");
}

void ACPP_M1A1_Pawn::BeginPlay()
{
	Super::BeginPlay();
	
	//바인딩
	if(IsValid(TankMovement))
	{
		TankMovement->TurretMoveStartFunc.BindUFunction(this,"TurretMoveLoop");
		TankMovement->TurretMoveEndFunc.BindUFunction(this,"TurretMoveEnd");
	}	
	if(IsValid(GunSystem))
	{
		GunSystem->FireEffectFunc.BindUFunction(this,"OnFireParticle");
		GunSystem->GunReloadDoneFunc.BindUFunction(this,"GunSystemSoundReloadDone");
	}
	
	PC = UGameplayStatics::GetPlayerController(this,0);
	
	//카메라
	Camera->SetActive(true);
	GunnerCam->SetActive(false);
	bUseControllerRotationYaw = false;
	//사운드
	IdleAudio->Play();
	IdleAudio->OnAudioFinished.AddDynamic(this,&ACPP_M1A1_Pawn::IdleSoundPlay);
	EngineAudio->OnAudioFinished.AddDynamic(this,&ACPP_M1A1_Pawn::EngineSoundStop);
	GunSystemAudio->OnAudioFinished.AddDynamic(this,&ACPP_M1A1_Pawn::GunSystemSoundStop);
	//Damage
	HP = MAX_HP;
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
	PlayerInputComponent->BindAction("ViewChange",IE_Pressed,this,&ACPP_M1A1_Pawn::CamChange);
	PlayerInputComponent->BindAction("Fire",IE_Pressed,this,&ACPP_M1A1_Pawn::OnMainGunFire);
}

void ACPP_M1A1_Pawn::Dead()
{
	Super::Dead();
	UMaterialInstanceConstant* DamageMat = Cast<UMaterialInstanceConstant>(UAssetManager::GetStreamableManager().LoadSynchronous(FSoftObjectPath(L"MaterialInstanceConstant'/Game/VigilanteContent/Vehicles/West_Tank_M1A1Abrams/Damaged/Materials/MI_West_Tank_M1A1Abrams_Damaged.MI_West_Tank_M1A1Abrams_Damaged'")));
	TankMesh->SetMaterial(0,DamageMat);
}

void ACPP_M1A1_Pawn::ParameterSet()
{
	//sight
	CamRange = 800;
	BasicCamTurnSpeed = 100;
	PitchLimitMax = 20;
	PitchLimitMin = -10;
	CamType = ECameraType::THIRD;
	//Damage
	MAX_HP = 100;
	HP = MAX_HP;
}

void ACPP_M1A1_Pawn::RootSet()
{
	//mesh
	TankRoot = CreateDefaultSubobject<UStaticMeshComponent>(L"TankRoot");
	RootComponent = TankRoot;
	TankMesh = CreateDefaultSubobject<USkeletalMeshComponent>(L"TankMesh");
	TankMesh->SetupAttachment(TankRoot);
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
}

void ACPP_M1A1_Pawn::CollisionSet()
{
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
	/*객체 초기화*/
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
}

void ACPP_M1A1_Pawn::CameraSet()
{
	//camera
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(L"SpringArm");
	SpringArm->SetupAttachment(TankMesh);
	Camera = CreateDefaultSubobject<UCameraComponent>(L"Camera");
	Camera->SetupAttachment(SpringArm);
	GunnerSpringArm = CreateDefaultSubobject<USpringArmComponent>("GunnerSpringArm");
	GunnerSpringArm->SetupAttachment(TankMesh);
	GunnerCam = CreateDefaultSubobject<UCameraComponent>(L"GunnerCam");
	GunnerCam->SetupAttachment(GunnerSpringArm);
	
	/*객체 초기화*/
	//camera
	SpringArm->SetRelativeLocation(FVector(0, 0, 260));
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->TargetArmLength = CamRange;
	Camera->SetRelativeLocation(FVector(0,0,200));
	GunnerSpringArm->SetRelativeLocation(FVector(0, 0, 0));
	GunnerSpringArm->bUsePawnControlRotation = true;
	GunnerSpringArm->TargetArmLength = 0;
	GunnerSpringArm->SetupAttachment(TankMesh,"GunnerCamPos");
	GunnerCam->SetRelativeLocation(FVector(0,0,20));
	GunnerCam->SetupAttachment(GunnerSpringArm);
}

void ACPP_M1A1_Pawn::ParticleSet()
{
	//particle
	MuzzleFlashEffect = CreateDefaultSubobject<UParticleSystemComponent>(L"MuzzleFlash");
	MuzzleFlashEffect->SetupAttachment(TankMesh);
	ShockWaveEffect = CreateDefaultSubobject<UParticleSystemComponent>(L"ShockWave");
	ShockWaveEffect->SetupAttachment(TankMesh);
	WheelsEffect.SetNum(8);
	for(int i =0;i<WheelsEffect.Num();i++)
	{
		FString name = FString::Printf(TEXT("Wheel%d"),i);
		WheelsEffect[i] = CreateDefaultSubobject<UParticleSystemComponent>(FName(name));
	}
	/*객체 초기화*/
	//particle
	ConstructorHelpers::FObjectFinder<UParticleSystem> MuzzleParticle
	(L"ParticleSystem'/Game/VigilanteContent/Vehicles/West_Tank_M1A1Abrams/FX/PS_MuzzleFire_01_M1A1Abrams.PS_MuzzleFire_01_M1A1Abrams'");
	MuzzleFlashEffect->Template = MuzzleParticle.Object;
	MuzzleFlashEffect->SetupAttachment(TankMesh,"gun_1_jntSocket");
	MuzzleFlashEffect->bAutoActivate = false;
	MuzzleFlashEffect->bAutoDestroy = false;
	ConstructorHelpers::FObjectFinder<UParticleSystem> ShockWaveParticle
	(L"ParticleSystem'/Game/VigilanteContent/Vehicles/West_Tank_M1A1Abrams/FX/PS_ShockWave_M1A1Abrams.PS_ShockWave_M1A1Abrams'");
	ShockWaveEffect->Template = ShockWaveParticle.Object;
	ShockWaveEffect->SetupAttachment(TankMesh,"root_jnt");
	ShockWaveEffect->bAutoActivate = false;
	ShockWaveEffect->bAutoDestroy = false;
	ConstructorHelpers::FObjectFinder<UParticleSystem> WheelParticle
	(L"ParticleSystem'/Game/VigilanteContent/Shared/Particles/ParticleSystems/PS_Dust_WheelTrack_03.PS_Dust_WheelTrack_03'");
	for(int i =0;i<WheelsEffect.Num();i++)
	{
		FString name = FString::Printf(TEXT("Wheel%d"),i);
		WheelsEffect[i]->SetupAttachment(TankMesh,FName(name));
		WheelsEffect[i]->Template = WheelParticle.Object;
		WheelsEffect[i]->bAutoActivate = false;
		WheelsEffect[i]->bAutoDestroy = false;
	}
}

void ACPP_M1A1_Pawn::SoundSet()
{
	EngineAudio = CreateDefaultSubobject<UAudioComponent>(L"EngineAudio");
	EngineAudio->SetupAttachment(Engine);
	IdleAudio = CreateDefaultSubobject<UAudioComponent>(L"IdleAudio");
	IdleAudio->SetupAttachment(TankMesh);
	GunSystemAudio = CreateDefaultSubobject<UAudioComponent>(L"GunSystemAudio");
	GunSystemAudio->SetupAttachment(GunnerCam);
	TurretSystemAudio = CreateDefaultSubobject<UAudioComponent>(L"TurretSystemAudio");
	TurretSystemAudio->SetupAttachment(GunnerCam);
	
	ConstructorHelpers::FObjectFinder<USoundAttenuation> EngineAttenuation
	(L"SoundAttenuation'/Game/BP/Sound/Attenuation/EngineSoundAttenuation.EngineSoundAttenuation'");
	EngineSoundAttenuation = EngineAttenuation.Object;
	IdleAudio->AttenuationSettings = EngineAttenuation.Object;
	EngineAudio->AttenuationSettings = EngineAttenuation.Object;
	ConstructorHelpers::FObjectFinder<USoundAttenuation> MainGunAttenuation
	(L"SoundAttenuation'/Game/BP/Sound/Attenuation/MainGunSoundAttenuation.MainGunSoundAttenuation'");
	GunSystemAudio->AttenuationSettings = MainGunAttenuation.Object;
	MainGunSoundAttenuation = MainGunAttenuation.Object;
	ConstructorHelpers::FObjectFinder<USoundAttenuation> TurretAttenuation
	(L"SoundAttenuation'/Game/BP/Sound/Attenuation/TurretSoundAttenuation.TurretSoundAttenuation'");
	TurretSoundAttenuation = TurretAttenuation.Object;
	TurretSystemAudio->AttenuationSettings = TurretSoundAttenuation;
	/*객체 초기화*/
	MainGunFireSound.SetNum(6);
	MainGunReloadDoneSound.SetNum(3);
	//Sound
	ConstructorHelpers::FObjectFinder<USoundWave> IdleStartCue
	(L"SoundWave'/Game/Sound/Tank/Engine/M1A2_Start_Idle_Stop_Wave_0_0_0.M1A2_Start_Idle_Stop_Wave_0_0_0'");
	IdleStartSound=IdleStartCue.Object;
	ConstructorHelpers::FObjectFinder<USoundWave> IdleLoopCue
	(L"SoundWave'/Game/Sound/Tank/Engine/M1A2_Start_Idle_Stop_Wave_0_0_1.M1A2_Start_Idle_Stop_Wave_0_0_1'");
	IdleLoopSound=IdleLoopCue.Object;
	ConstructorHelpers::FObjectFinder<USoundWave> IdleEndCue
	(L"SoundWave'/Game/Sound/Tank/Engine/M1A2_Start_Idle_Stop_Wave_0_0_2.M1A2_Start_Idle_Stop_Wave_0_0_2'");
	IdleEndSound=IdleEndCue.Object;
	ConstructorHelpers::FObjectFinder<USoundWave> EngineStartCue
	(L"SoundWave'/Game/Sound/Tank/Engine/M1A2_SuperTurbine_Wave_0_0_0.M1A2_SuperTurbine_Wave_0_0_0'");
	EngineStartSound=EngineStartCue.Object;
	ConstructorHelpers::FObjectFinder<USoundWave> EngineLoopCue
	(L"SoundWave'/Game/Sound/Tank/Engine/M1A2_SuperTurbine_Wave_0_0_1.M1A2_SuperTurbine_Wave_0_0_1'");
	EngineLoopSound=EngineLoopCue.Object;
	ConstructorHelpers::FObjectFinder<USoundWave> EngineEndCue
	(L"SoundWave'/Game/Sound/Tank/Engine/M1A2_SuperTurbine_Wave_0_0_2.M1A2_SuperTurbine_Wave_0_0_2'");
	EngineEndSound=EngineEndCue.Object;
	
	ConstructorHelpers::FObjectFinder<USoundCue> MainGunFireCue0
	(L"SoundCue'/Game/Sound/Tank/Fire/120mm_Cannon_Fire_Close_Wave_0_0_0_Cue.120mm_Cannon_Fire_Close_Wave_0_0_0_Cue'");
	MainGunFireSound[0]=MainGunFireCue0.Object;
	ConstructorHelpers::FObjectFinder<USoundCue> MainGunFireCue1
	(L"SoundCue'/Game/Sound/Tank/Fire/120mm_Cannon_Fire_Close_Wave_1_0_0_Cue.120mm_Cannon_Fire_Close_Wave_1_0_0_Cue'");
	MainGunFireSound[1]=MainGunFireCue1.Object;
	ConstructorHelpers::FObjectFinder<USoundCue> MainGunFireCue2
	(L"SoundCue'/Game/Sound/Tank/Fire/120mm_Cannon_HiFi_Wave_0_0_0_Cue.120mm_Cannon_HiFi_Wave_0_0_0_Cue'");
	MainGunFireSound[2]=MainGunFireCue2.Object;
	ConstructorHelpers::FObjectFinder<USoundCue> MainGunFireCue3
	(L"SoundCue'/Game/Sound/Tank/Fire/120mm_Cannon_HiFi_Wave_1_0_0_Cue.120mm_Cannon_HiFi_Wave_1_0_0_Cue'");
	MainGunFireSound[3]=MainGunFireCue3.Object;
	ConstructorHelpers::FObjectFinder<USoundCue> MainGunFireCue4
	(L"SoundCue'/Game/Sound/Tank/Fire/120mm_Cannon_HiFi_Wave_2_0_0_Cue.120mm_Cannon_HiFi_Wave_2_0_0_Cue'");
	MainGunFireSound[4]=MainGunFireCue4.Object;
	ConstructorHelpers::FObjectFinder<USoundCue> MainGunFireCue5
	(L"SoundCue'/Game/Sound/Tank/Fire/120mm_Cannon_HiFi_Wave_3_0_0_Cue.120mm_Cannon_HiFi_Wave_3_0_0_Cue'");
	MainGunFireSound[5]=MainGunFireCue5.Object;
	
	ConstructorHelpers::FObjectFinder<USoundCue> MainGunReloadCue
	(L"SoundCue'/Game/Sound/Tank/Reload/120mm_Cannon_Reload_M1A2_Wave_0_0_0_Cue.120mm_Cannon_Reload_M1A2_Wave_0_0_0_Cue'");
	MainGunReloadSound=MainGunReloadCue.Object;
	ConstructorHelpers::FObjectFinder<USoundCue> MainGunReloadDoneCue0
	(L"SoundCue'/Game/Sound/Tank/Reload/120mm_Cannon_Reload_M1A2_End_Wave_0_0_0_Cue.120mm_Cannon_Reload_M1A2_End_Wave_0_0_0_Cue'");
	MainGunReloadDoneSound[0]=MainGunReloadDoneCue0.Object;
	ConstructorHelpers::FObjectFinder<USoundCue> MainGunReloadDoneCue1
	(L"SoundCue'/Game/Sound/Tank/Reload/120mm_Cannon_Reload_M1A2_End_Wave_1_0_0_Cue.120mm_Cannon_Reload_M1A2_End_Wave_1_0_0_Cue'");
	MainGunReloadDoneSound[1]=MainGunReloadDoneCue1.Object;
	ConstructorHelpers::FObjectFinder<USoundCue> MainGunReloadDoneCue2
	(L"SoundCue'/Game/Sound/Tank/Reload/120mm_Cannon_Reload_M1A2_End_Wave_2_0_0_Cue.120mm_Cannon_Reload_M1A2_End_Wave_2_0_0_Cue'");
	MainGunReloadDoneSound[2]=MainGunReloadDoneCue2.Object;

	ConstructorHelpers::FObjectFinder<USoundWave> TurretLoop
	(L"SoundWave'/Game/Sound/Tank/Turret/Turret_Loop_01_Wave_0_0_0.Turret_Loop_01_Wave_0_0_0'");
	TurretLoopSound = TurretLoop.Object;
	ConstructorHelpers::FObjectFinder<USoundWave> TurretEnd
	(L"SoundWave'/Game/Sound/Tank/Turret/Turret_Start_Stop_Wave_0_3_0.Turret_Start_Stop_Wave_0_3_0'");
	TurretEndSound = TurretEnd.Object;
	
	IdleAudio->Sound = IdleStartSound;
	IdleAudio->VolumeMultiplier = 0.2f;
	EngineAudio->Sound = EngineStartSound;
	EngineAudio->VolumeMultiplier = 0.3f;
	GunSystemAudio->VolumeMultiplier=0.5f;
	TurretSystemAudio->VolumeMultiplier=0.3f;
}





