#include "GameInstance/CPP_MultiplayGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Common/UObject/Manager/ObjectPool/CPP_ObjectPoolManager.h"

UCPP_MultiplayGameInstance::UCPP_MultiplayGameInstance()
{
}

void UCPP_MultiplayGameInstance::BeginPlay()
{
	
}

void UCPP_MultiplayGameInstance::Init()
{//플레이 시작할때 
	Super::Init();
	//objectpoolmanager 저장
	RegisterManagerClass(UCPP_ObjectPoolManager::StaticClass());
}

void UCPP_MultiplayGameInstance::Shutdown()
{//플레이 엄출때
	for (auto pair : ManagerClasses)
	{
		if(!IsValid(pair.Value))
			return;
		//IsValid 누군가 가리키는 오브젝트 인지?
		//IsValidLowlevel,Fast 현재 레벨에 남아있는지?
		if (!pair.Value->IsValidLowLevel()) return;
		
		pair.Value->ShutdownManagerClass();

		// 객체 소멸
		pair.Value->ConditionalBeginDestroy();
	}
	ManagerClasses.Empty();
	
	Super::Shutdown();
}

void UCPP_MultiplayGameInstance::RegisterManagerClass(TSubclassOf<UCPP_UManagerClass> managerClass)
{
	UCPP_UManagerClass* managerClassInst = NewObject<UCPP_UManagerClass>(this,managerClass,NAME_None,EObjectFlags::RF_MarkAsRootSet);
	//가비지 컬랙터가 호출되지 않도록 제작
	managerClassInst->InitManagerClass();
	//TMap에 저장
	ManagerClasses.Add(managerClass->GetName(), managerClassInst);
	UE_LOG(LogTemp,Display,L"%s",*managerClass->GetName());
}
