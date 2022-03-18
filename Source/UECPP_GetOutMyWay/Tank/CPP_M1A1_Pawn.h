#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CPP_M1A1_Pawn.generated.h"

DECLARE_DELEGATE(FFire);

UENUM(BlueprintType)
enum class ECameraType : uint8
{
	THIRD		UMETA(DisplayName = "Third"),
	GUNNER		UMETA(DisplayName = "Gunner"),
	MAX
};

UCLASS()
class UECPP_GETOUTMYWAY_API ACPP_M1A1_Pawn : public APawn
{
	GENERATED_BODY()
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

	class UCPP_M1A1MainGunSystemComponent* GunSystem;

	class UCPP_ParticleControlComponent* ParticleSystem;
public:
	ACPP_M1A1_Pawn();

	FFire FireFunc;
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	//get&set
	//기존의 getmovementcomp 함수를 현재가지고 있는 함수로 교체하는 과정
	FORCEINLINE virtual UPawnMovementComponent* GetMovementComponent() const override;
	
private:
	//Setup
	void RootSet();
	void CollisionSet();
	void CameraSet();
	void ParticleSet();
	void SoundSet();
	
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
	void GunSystemSoundStop();
	UFUNCTION()
	void GunSystemSoundReloadDone();
	UFUNCTION()
	void TurretMoveLoop();
	UFUNCTION()
	void TurretMoveEnd();
	//Damage

	
private:
	//sight
	float CamRange = 800;
	float BasicCamTurnSpeed = 100;
	float PitchLimitMax = 20;
	float PitchLimitMin = -10;
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
	
};
