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

	template<typename ManagerClassType>
	FORCEINLINE ManagerClassType* GetManagerClass()
	{return Cast<ManagerClassType>(ManagerClasses[ManagerClassType::StaticClass()->GetName()]);}
protected:
	virtual void Init() override;
	virtual void Shutdown() override;
	
	void RegisterManagerClass(TSubclassOf<UCPP_UManagerClass> managerClass);

	TMap<FString,UCPP_UManagerClass*> ManagerClasses;
};
