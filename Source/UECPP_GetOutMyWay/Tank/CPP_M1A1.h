#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CPP_M1A1.generated.h"

UCLASS()
class UECPP_GETOUTMYWAY_API ACPP_M1A1 : public ACharacter
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly)
		class UCameraComponent* Camera;
	UPROPERTY(EditDefaultsOnly)
		class USpringArmComponent* SpringArm;

public:
	ACPP_M1A1();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
private:
	float CamRange = 800;

};
