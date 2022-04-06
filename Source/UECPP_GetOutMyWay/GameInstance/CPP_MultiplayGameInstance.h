#pragma once

#include "CoreMinimal.h"
#include "Common/UObject/Manager/CPP_UManagerClass.h"
#include "Engine/GameInstance.h"
#include "CPP_MultiplayGameInstance.generated.h"

UCLASS()
class UECPP_GETOUTMYWAY_API UCPP_MultiplayGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UCPP_MultiplayGameInstance();

	void BeginPlay();
protected:
	virtual void Init() override;

	
	void RegisterManagerClass(TSubclassOf<UCPP_UManagerClass> managerClass);

	TMap<FString,UCPP_UManagerClass*> ManagerClasses;
};
