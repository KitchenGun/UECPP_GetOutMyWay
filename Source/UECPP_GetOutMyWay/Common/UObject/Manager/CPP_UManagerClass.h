#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CPP_UManagerClass.generated.h"

UCLASS(Abstract)
class UECPP_GETOUTMYWAY_API UCPP_UManagerClass : public UObject
{
	GENERATED_BODY()
public:

	FORCEINLINE virtual void InitManagerClass() {};
	FORCEINLINE virtual void ShutdownManagerClass() {};
	FORCEINLINE virtual void BeginPlay() {};
};
