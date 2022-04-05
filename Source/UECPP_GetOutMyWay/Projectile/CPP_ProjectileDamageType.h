
#pragma once

#include "CoreMinimal.h"
#include "CPP_Projectile.h"
#include "UObject/Interface.h"
#include "CPP_ProjectileDamageType.generated.h"

UINTERFACE(MinimalAPI)
class UCPP_ProjectileDamageType : public UInterface
{
	GENERATED_BODY()
};

class UECPP_GETOUTMYWAY_API ICPP_ProjectileDamageType
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual void BounceCal(float hitAngle,EHitDir hitDir) PURE_VIRTUAL(ICPP_ProjectileDamageType::BounceCal,);
};
