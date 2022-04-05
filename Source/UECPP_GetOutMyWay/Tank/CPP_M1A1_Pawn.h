#pragma once

#include "CoreMinimal.h"
#include "CPP_Tank_Pawn.h"
#include "CPP_M1A1_Pawn.generated.h"


UCLASS()
class UECPP_GETOUTMYWAY_API ACPP_M1A1_Pawn : public ACPP_Tank_Pawn
{
	GENERATED_BODY()
public:
	ACPP_M1A1_Pawn();

protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Dead() override;
private:
	//Setup
	void ParameterSet();
	void RootSet();
	void CollisionSet();
	void CameraSet();
	void ParticleSet();
	void SoundSet();

	
	
};
