#pragma once

#include "CoreMinimal.h"
#include "Tank/Component/CPP_MainGunSystemComponent.h"
#include "CPP_M1A1MainGunSystemComponent.generated.h"

DECLARE_DELEGATE(FFireEffect)

UCLASS()
class UECPP_GETOUTMYWAY_API UCPP_M1A1MainGunSystemComponent : public UCPP_MainGunSystemComponent
{
	GENERATED_BODY()
	
public:
	UCPP_M1A1MainGunSystemComponent();
	
	void MainGunFire() override;
	FFireEffect FireEffectFunc;
protected:
	void BeginPlay() override;
private:
	class ACPP_M1A1_Pawn* Owner;
	float M1A1ReloadTime =6.0f;
};
