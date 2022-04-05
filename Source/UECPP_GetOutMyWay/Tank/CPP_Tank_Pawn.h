#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CPP_Tank_Pawn.generated.h"


DECLARE_DELEGATE(FFire);

UENUM(BlueprintType)
enum class ECameraType : uint8
{
	THIRD		UMETA(DisplayName = "Third"),
	GUNNER		UMETA(DisplayName = "Gunner"),
	MAX
};

UCLASS()
class UECPP_GETOUTMYWAY_API ACPP_Tank_Pawn : public APawn
{
	GENERATED_BODY()

public:
	ACPP_Tank_Pawn();
protected:
	//Root
	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* TankRoot;

	//Mesh
	UPROPERTY(EditDefaultsOnly)
	class USkeletalMeshComponent* TankMesh;

	//Camera
	UPROPERTY(EditDefaultsOnly)
	class UCameraComponent* Camera;
	UPROPERTY(EditDefaultsOnly)
	class UCameraComponent* GunnerCam;
	UPROPERTY(EditDefaultsOnly)
	class USpringArmComponent* GunnerSpringArm;
	UPROPERTY(EditDefaultsOnly)
	class USpringArmComponent* SpringArm;
	
	//Collision
	UPROPERTY(VisibleDefaultsOnly)
	class UBoxComponent* FrontUpper;
	UPROPERTY(VisibleDefaultsOnly)
	class UBoxComponent* FrontUnder;
	UPROPERTY(VisibleDefaultsOnly)
	class UBoxComponent* Engine;
	UPROPERTY(VisibleDefaultsOnly)
	class UBoxComponent* Bottom;
	UPROPERTY(VisibleDefaultsOnly)
	class UBoxComponent* LSide;
	UPROPERTY(VisibleDefaultsOnly)
	class UBoxComponent* RSide;
	UPROPERTY(VisibleDefaultsOnly)
	class UBoxComponent* Turret;

	//Particle
	UPROPERTY(EditDefaultsOnly)
	class UParticleSystemComponent* MuzzleFlashEffect;
	UPROPERTY(EditDefaultsOnly)
	class UParticleSystemComponent* ShockWaveEffect;
	UPROPERTY(EditDefaultsOnly)
	TArray<class UParticleSystemComponent*> WheelsEffect;

	//sound
	UPROPERTY(VisibleDefaultsOnly)
	class UAudioComponent* EngineAudio;
	UPROPERTY(VisibleDefaultsOnly)
	class UAudioComponent* IdleAudio;
	UPROPERTY(VisibleDefaultsOnly)
	class UAudioComponent* GunSystemAudio;
	UPROPERTY(VisibleDefaultsOnly)
	class UAudioComponent* TurretSystemAudio;

	//ActorComp
	class UCPP_TrackMovementComponent* TrackMovement;

	class UCPP_TankPawnMovementComponent* TankMovement;

	class UCPP_MainGunSystemComponent* GunSystem;

	class UCPP_ParticleControlComponent* ParticleSystem;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	//get&set
	FORCEINLINE UParticleSystemComponent* GetMuzzleFlashEffect() {return MuzzleFlashEffect;}
	FORCEINLINE UParticleSystemComponent* GetShockWaveEffect() {return ShockWaveEffect;}
	FORCEINLINE TArray<class UParticleSystemComponent*> GetWheelsEffect() {return WheelsEffect;}

	FORCEINLINE float GetGunAngleOffset() {return displacementAngle;}

	//Delegate
	FFire FireFunc;
protected:
	virtual void BeginPlay() override;

	//sight
	void OnVerticalLook(float value);
	void OnHorizontalLook(float value);
	void CamPitchLimitSmooth();
	void CamChange();
	//move
	void OnMoveForward(float value);
	void OnMoveTurn(float value);
	void OnEngineBreak();
	void OffEngineBreak();
	//action
	void OnMainGunFire();
	//particle
	void OnWheelParticle();
	UFUNCTION()
	void OnFireParticle();
	//Sound
	UFUNCTION()
	void IdleSoundPlay();
	void EngineSoundPlay();
	UFUNCTION()
	void EngineSoundStop();
	void GunSystemSoundPlay();
	UFUNCTION()
	virtual void GunSystemSoundStop();
	UFUNCTION()
	virtual void GunSystemSoundReloadDone();
	UFUNCTION()
	virtual void TurretMoveLoop();
	UFUNCTION()
	virtual void TurretMoveEnd();

	//Dead
	UFUNCTION()
	virtual void Dead();
	
protected:
	//////////////////////////////tank 파라미터 변수/////////////////////////////////////////
	//sight
	float CamRange = 800;
	float BasicCamTurnSpeed = 100;
	float PitchLimitMax = 20;
	float PitchLimitMin = -10;
	float displacementAngle =0.0f;
	ECameraType CamType = ECameraType::THIRD;
	
	//APlayerController
	APlayerController* PC = nullptr;

	//sound
	class USoundWave* EngineStartSound;
	class USoundWave* EngineLoopSound;
	class USoundWave* EngineEndSound;
	
	class USoundWave* IdleStartSound;
	class USoundWave* IdleLoopSound;
	class USoundWave* IdleEndSound;

	class USoundWave* TurretLoopSound;
	class USoundWave* TurretEndSound;
	
	TArray<class USoundCue*> MainGunFireSound;
	class USoundCue* MainGunReloadSound;
	TArray<class USoundCue*> MainGunReloadDoneSound;
	bool IsMoveBefore=false;
	bool IsEngineEnd = true;
	bool IsGunReloadDone=true;

	class USoundAttenuation* MainGunSoundAttenuation;
	class USoundAttenuation* EngineSoundAttenuation;
	class USoundAttenuation* TurretSoundAttenuation;

	//Damage
	float MAX_HP = 100;
	float HP = MAX_HP;
	bool IsDead = false;
};
