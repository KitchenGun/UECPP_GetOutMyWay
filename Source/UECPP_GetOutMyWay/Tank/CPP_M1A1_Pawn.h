#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CPP_M1A1_Pawn.generated.h"

UCLASS()
class UECPP_GETOUTMYWAY_API ACPP_M1A1_Pawn : public APawn
{
	GENERATED_BODY()

public:
	ACPP_M1A1_Pawn();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
