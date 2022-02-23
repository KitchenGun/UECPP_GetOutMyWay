#include "Tank/CPP_M1A1_Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
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


ACPP_M1A1_Pawn::ACPP_M1A1_Pawn()
{
	PrimaryActorTick.bCanEverTick = true;
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
}

void ACPP_M1A1_Pawn::BeginPlay()
{
	Super::BeginPlay();
	PC = UGameplayStatics::GetPlayerController(this,0);
	//바인딩
	if(IsValid(GunSystem))
	{
		GunSystem->FireEffectFunc.BindUFunction(this,"OnFireParticle");
		GunSystem->GunReloadDoneFunc.BindUFunction(this,"GunSystemSoundReloadDone");
	}
	if(IsValid(TankMovement))
	{
		TankMovement->TurretMoveStartFunc.BindUFunction(this,"TurretMoveLoop");
		TankMovement->TurretMoveEndFunc.BindUFunction(this,"TurretMoveEnd");
	}	
	//카메라
	Camera->SetActive(true);
	GunnerCam->SetActive(false);
	bUseControllerRotationYaw = false;
	//사운드
	IdleAudio->Play();
	IdleAudio->OnAudioFinished.AddDynamic(this,&ACPP_M1A1_Pawn::IdleSoundPlay);
	EngineAudio->OnAudioFinished.AddDynamic(this,&ACPP_M1A1_Pawn::EngineSoundStop);
	GunSystemAudio->OnAudioFinished.AddDynamic(this,&ACPP_M1A1_Pawn::GunSystemSoundStop);
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

UPawnMovementComponent* ACPP_M1A1_Pawn::GetMovementComponent() const
{
	return TankMovement;
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
	GunnerSpringArm->AttachToComponent(TankMesh,FAttachmentTransformRules::KeepWorldTransform,"GunnerCamPos");
	GunnerCam->SetRelativeLocation(FVector(0,0,20));
	GunnerCam->AttachToComponent(GunnerSpringArm,FAttachmentTransformRules::KeepRelativeTransform);
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
	MuzzleFlashEffect->AttachToComponent(TankMesh,FAttachmentTransformRules::KeepWorldTransform,"gun_1_jntSocket");
	MuzzleFlashEffect->bAutoActivate = false;
	MuzzleFlashEffect->bAutoDestroy = false;
	ConstructorHelpers::FObjectFinder<UParticleSystem> ShockWaveParticle
	(L"ParticleSystem'/Game/VigilanteContent/Vehicles/West_Tank_M1A1Abrams/FX/PS_ShockWave_M1A1Abrams.PS_ShockWave_M1A1Abrams'");
	ShockWaveEffect->Template = ShockWaveParticle.Object;
	ShockWaveEffect->AttachToComponent(TankMesh,FAttachmentTransformRules::KeepRelativeTransform,"root_jnt");
	ShockWaveEffect->bAutoActivate = false;
	ShockWaveEffect->bAutoDestroy = false;
	ConstructorHelpers::FObjectFinder<UParticleSystem> WheelParticle
	(L"ParticleSystem'/Game/VigilanteContent/Shared/Particles/ParticleSystems/PS_Dust_WheelTrack_03.PS_Dust_WheelTrack_03'");
	for(int i =0;i<WheelsEffect.Num();i++)
	{
		FString name = FString::Printf(TEXT("Wheel%d"),i);
		WheelsEffect[i]->AttachToComponent(TankMesh,FAttachmentTransformRules::KeepRelativeTransform,FName(name));
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
	GunSystemAudio->SetupAttachment(TankMesh);
	TurretSystemAudio = CreateDefaultSubobject<UAudioComponent>(L"TurretSystemAudio");
	TurretSystemAudio->SetupAttachment(TankMesh);
	
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
	//max를 수정해야함
	if (pitch < PitchLimitMin&&pitch>180)
	{//범위를 벗어난 상태
		pitch = FMath::Clamp(pitch, PitchLimitMin, 360.0f);
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

void ACPP_M1A1_Pawn::CamChange()
{
	static_cast<ECameraType>((uint8)CamType+1)==ECameraType::MAX
		?CamType=static_cast<ECameraType>((uint8)0)
		:CamType=static_cast<ECameraType>((uint8)CamType+1);
	UE_LOG(LogTemp,Display,L"CamType::%s",*UEnum::GetValueAsString(CamType));

	switch (CamType)
	{
	case ECameraType::THIRD:
		Camera->SetActive(true);
		GunnerCam->SetActive(false);
		break;
	case ECameraType::GUNNER:
		GunnerCam->SetActive(true);
		Camera->SetActive(false);
		break;
	}
	
}

void ACPP_M1A1_Pawn::OnMoveForward(float value)
{
	if (TankMovement != nullptr)
	{
		TankMovement->OnMove(value);
		OnWheelParticle();
	}
}

void ACPP_M1A1_Pawn::OnMoveTurn(float value)
{
	if (TankMovement != nullptr)
	{
		TankMovement->OnTurn(value);
		OnWheelParticle();
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

void ACPP_M1A1_Pawn::OnMainGunFire()
{
	if(FireFunc.IsBound())
		FireFunc.Execute();
}

void ACPP_M1A1_Pawn::OnWheelParticle()
{
	if(TankMovement->GetIsMove())
	{
		EngineSoundPlay();
		for(int i =0;i<WheelsEffect.Num();i++)
		{
			WheelsEffect[i]->SetActive(true);
		}
		IsMoveBefore = true;
	}
	else
	{
		EngineSoundStop();
		for(int i =0;i<WheelsEffect.Num();i++)
		{
			WheelsEffect[i]->SetActive(false);
		}
		IsMoveBefore = false;
	}
	
}

void ACPP_M1A1_Pawn::OnFireParticle()
{
	MuzzleFlashEffect->Activate(true);
	ShockWaveEffect->Activate(true);
	GunSystemSoundPlay();
	FVector Start = MuzzleFlashEffect->GetComponentLocation();
	FVector End = MuzzleFlashEffect->GetComponentLocation();
	TArray<AActor*> ignore;
	//ignore.Add(this);
	TArray<FHitResult> HitResults;
	TArray<AActor*> ImpactArray;
	float blastRange = 500;
	float ShockWaveForce=4e+3;
	const bool Hit =
		UKismetSystemLibrary::SphereTraceMulti(GetWorld(),Start,End,blastRange,
			ETraceTypeQuery::TraceTypeQuery8,false,ignore,EDrawDebugTrace::ForDuration,HitResults,true);
	if(Hit)
	{
		int32 index;
		for(FHitResult temp:HitResults)
		{
			AActor* tempActor=Cast<AActor>(temp.Actor);
			if(IsValid(tempActor))
			{
				ImpactArray.Find(tempActor,index);
				if(index==INDEX_NONE)
				{
					ImpactArray.Add(tempActor);
					UStaticMeshComponent* MeshComponent = Cast<UStaticMeshComponent>(tempActor->GetRootComponent());
					if(IsValid(MeshComponent)&&MeshComponent->BodyInstance.bSimulatePhysics)
					{
						MeshComponent->AddImpulse(FVector(MeshComponent->GetComponentLocation()-(Start-FVector(0,200,0)))*ShockWaveForce);
					}
				}
			}
		}
	}
	
}

void ACPP_M1A1_Pawn::IdleSoundPlay()
{
	IdleAudio->Sound = IdleLoopSound;
	IdleAudio->Play();
}

void ACPP_M1A1_Pawn::EngineSoundPlay()
{
	if(!IsMoveBefore&&TankMovement->GetIsMove())
	{
		EngineAudio->Sound = EngineStartSound;
		EngineAudio->Play();
		IsEngineEnd=false;
	}
}

void ACPP_M1A1_Pawn::EngineSoundStop()
{
	if(!TankMovement->GetIsMove())
	{
		EngineAudio->Sound = EngineEndSound;
		if(!IsEngineEnd)
		{
			EngineAudio->Stop();
			EngineAudio->Play();
		}
		IsEngineEnd = true;
	}
	else if(IsMoveBefore&&TankMovement->GetIsMove())
	{
		EngineAudio->Sound = EngineLoopSound;
		EngineAudio->Play();
	}
	
	
}

void ACPP_M1A1_Pawn::GunSystemSoundPlay()
{
	GunSystemAudio->AttenuationSettings = MainGunSoundAttenuation;
	if(CamType==ECameraType::THIRD)
	{
		GunSystemAudio->Sound = MainGunFireSound[UKismetMathLibrary::RandomIntegerInRange(0,1)];
		GunSystemAudio->Play();
	}
	else if(CamType==ECameraType::GUNNER)
	{
		GunSystemAudio->Sound = MainGunFireSound[UKismetMathLibrary::RandomIntegerInRange(2,5)];
		GunSystemAudio->Play();
	}
	
}

void ACPP_M1A1_Pawn::GunSystemSoundStop()
{
	if(!GunSystem->GetIsMainGunCanFire())
	{
		GunSystemAudio->AttenuationSettings = TurretSoundAttenuation;
		GunSystemAudio->Sound= MainGunReloadDoneSound[UKismetMathLibrary::RandomIntegerInRange(0,2)];
		GunSystemAudio->Play();
	}
}

void ACPP_M1A1_Pawn::GunSystemSoundReloadDone()
{
	GunSystemAudio->AttenuationSettings = TurretSoundAttenuation;
	GunSystemAudio->Sound= MainGunReloadSound;
	GunSystemAudio->Play();
}

void ACPP_M1A1_Pawn::TurretMoveLoop()
{
	TurretSystemAudio->Sound = TurretLoopSound;
	TurretSystemAudio->Play();
}

void ACPP_M1A1_Pawn::TurretMoveEnd()
{
	if(TurretSystemAudio->Sound==TurretLoopSound)
	{
		TurretSystemAudio->Sound = TurretEndSound;
		TurretSystemAudio->Play();
	}
	
}



