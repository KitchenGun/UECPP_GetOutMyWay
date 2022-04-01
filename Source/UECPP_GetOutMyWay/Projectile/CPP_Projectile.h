#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPP_Projectile.generated.h"

UENUM()
enum EHitDir
{
	Front,
	Side,
	Back,
	UpSide,
	DownSide,
	Max
};

UCLASS()
class UECPP_GETOUTMYWAY_API ACPP_Projectile : public AActor
{
	GENERATED_BODY()
	
public:	
	ACPP_Projectile();

	FORCEINLINE	void SetEventInstigator(FString objName,AController* playerCtrl)
	{
		ObjName=objName;
		PlayerCtrl = playerCtrl;
	}
	
protected:
	virtual void BeginPlay() override;

    float GetHitAngle(UPrimitiveComponent* HitComponent, UPrimitiveComponent* OtherComp, const FHitResult& Hit);

	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
protected:
	//데미지
	float Damage = 20.0f;
	//발사정보
	FString ObjName;
	AController* PlayerCtrl = nullptr;
	//피격 방향
	EHitDir ProjectileHitDir = EHitDir::Max;

	TSubclassOf<UDamageType> DamageType = nullptr;
private:
	UPROPERTY(VisibleDefaultsOnly)
	class UCapsuleComponent* Capsule;
	UPROPERTY(VisibleDefaultsOnly)
	class UStaticMeshComponent* Shell;
	UPROPERTY(VisibleDefaultsOnly)
	class UStaticMeshComponent* WarHead;
	UPROPERTY(VisibleDefaultsOnly)
	class UStaticMeshComponent* Effect;
	//방향
	FVector StartPos=FVector::ZeroVector;
	
	class UProjectileMovementComponent* ProjectileMovement;
};
