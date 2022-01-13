#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
//생성에 필요한 헤더
#include "Components/SkeletalMeshComponent.h"

#include "M1A1.generated.h"

UCLASS()
class UECPP_GETOUTMYWAY_API AM1A1 : public ACharacter
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere)
		class USkeletalMeshComponent* TankMesh;

public:
	AM1A1();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
