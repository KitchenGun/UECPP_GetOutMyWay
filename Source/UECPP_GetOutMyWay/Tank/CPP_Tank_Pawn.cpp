#include "Tank/CPP_Tank_Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
//mesh
#include "Components/BoxComponent.h"
//camera
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
//sound
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
//actorComp
#include "Component/CPP_TrackMovementComponent.h"
#include "Component/CPP_TankPawnMovementComponent.h"
#include "Component/CPP_M1A1MainGunSystemComponent.h"
#include "Component/CPP_ParticleControlComponent.h"
#include "Kismet/KismetMathLibrary.h"

ACPP_Tank_Pawn::ACPP_Tank_Pawn()
{
	PrimaryActorTick.bCanEverTick = true;
	
}

void ACPP_Tank_Pawn::BeginPlay()
{
	Super::BeginPlay();
}

void ACPP_Tank_Pawn::OnVerticalLook(float value)
{
	AddControllerPitchInput(value * BasicCamTurnSpeed * GetWorld()->DeltaTimeSeconds);	
}

void ACPP_Tank_Pawn::OnHorizontalLook(float value)
{
	AddControllerYawInput(value * BasicCamTurnSpeed * GetWorld()->DeltaTimeSeconds);
}

void ACPP_Tank_Pawn::CamPitchLimitSmooth()
{
	float pitch = Controller->GetControlRotation().Quaternion().Rotator().Pitch;
	float minAngle = PitchLimitMin;
	float maxAngle = PitchLimitMax;
	//탱크의 pitch를 구해서 등판각을 받음
	if(!FMath::IsNearlyZero(Turret->GetComponentRotation().Pitch,0.1f))
	{
		displacementAngle = FRotator(GunnerSpringArm->GetComponentRotation().Quaternion()).Pitch;
		minAngle = PitchLimitMin+displacementAngle;
		maxAngle = PitchLimitMax+displacementAngle;
	}
	else
	{
		displacementAngle =0;
	}
	pitch = FMath::Clamp(pitch, minAngle, maxAngle);
	
	FRotator temp = FRotator(pitch, Controller->GetControlRotation().Quaternion().Rotator().Yaw, Controller->GetControlRotation().Quaternion().Rotator().Roll);
	Controller->SetControlRotation(temp);
}

void ACPP_Tank_Pawn::CamChange()
{
	static_cast<ECameraType>((uint8)CamType+1)==ECameraType::MAX
	?CamType=static_cast<ECameraType>((uint8)0)
	:CamType=static_cast<ECameraType>((uint8)CamType+1);

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

void ACPP_Tank_Pawn::OnMoveForward(float value)
{
	if (TankMovement != nullptr)
	{
		TankMovement->OnMove(value);
		OnWheelParticle();
	}
}

void ACPP_Tank_Pawn::OnMoveTurn(float value)
{
	if (TankMovement != nullptr)
	{
		TankMovement->OnTurn(value);
		OnWheelParticle();
	}
}

void ACPP_Tank_Pawn::OnEngineBreak()
{
	TankMovement->OnEngineBreak();
}

void ACPP_Tank_Pawn::OffEngineBreak()
{
	TankMovement->OffEngineBreak();
}

void ACPP_Tank_Pawn::OnMainGunFire()
{
	if(FireFunc.IsBound())
		FireFunc.Execute();
}

void ACPP_Tank_Pawn::OnWheelParticle()
{
	if(ParticleSystem->OnWheelParticle(TankMovement->GetIsMove()))
	{
		EngineSoundPlay();
		IsMoveBefore =true;
	}
	else if(IsMoveBefore)
	{
		EngineSoundStop();
		IsMoveBefore = false;
	}
	
}

void ACPP_Tank_Pawn::OnFireParticle()
{
	ParticleSystem->OnFireParticle();
	GunSystemSoundPlay();
}

void ACPP_Tank_Pawn::IdleSoundPlay()
{
	IdleAudio->Sound = IdleLoopSound;
	IdleAudio->Play();
}

void ACPP_Tank_Pawn::EngineSoundPlay()
{
	if(!IsMoveBefore&&TankMovement->GetIsMove())
	{
		EngineAudio->Sound = EngineStartSound;
		EngineAudio->Play();
		IsEngineEnd=false;
	}
}

void ACPP_Tank_Pawn::EngineSoundStop()
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

void ACPP_Tank_Pawn::GunSystemSoundPlay()
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

void ACPP_Tank_Pawn::GunSystemSoundStop()
{
	if (!GunSystem->GetIsMainGunCanFire())
	{
		GunSystemAudio->AttenuationSettings = TurretSoundAttenuation;
		GunSystemAudio->Sound = MainGunReloadDoneSound[UKismetMathLibrary::RandomIntegerInRange(0, 2)];
		GunSystemAudio->Play();
	}
}

void ACPP_Tank_Pawn::GunSystemSoundReloadDone()
{
	GunSystemAudio->AttenuationSettings = TurretSoundAttenuation;
	GunSystemAudio->Sound= MainGunReloadSound;
	GunSystemAudio->Play();
}

void ACPP_Tank_Pawn::TurretMoveLoop()
{
	TurretSystemAudio->Sound = TurretLoopSound;
	TurretSystemAudio->Play();
}

void ACPP_Tank_Pawn::TurretMoveEnd()
{
	if (TurretSystemAudio->Sound == TurretLoopSound)
	{
		TurretSystemAudio->Sound = TurretEndSound;
		TurretSystemAudio->Play();
	}
}

void ACPP_Tank_Pawn::Dead()
{
	IsDead =true;
}


void ACPP_Tank_Pawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//Turret Collider 위치 변환 탱크 포탑에 충돌체 같이 움직이기 위해서
	Turret->SetWorldLocation(TankMesh->GetSocketLocation("turret_jntSocket"));
	Turret->SetWorldRotation(TankMesh->GetSocketRotation("turret_jntSocket"));
}

void ACPP_Tank_Pawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float ACPP_Tank_Pawn::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	HP-=DamageAmount;

	if(!IsDead&&HP<=0)
	{
		Dead();
	}
	
	UE_LOG(LogTemp,Display,L"%.2f",HP);
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}




