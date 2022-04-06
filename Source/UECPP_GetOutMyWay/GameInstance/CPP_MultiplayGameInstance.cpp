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
{
	Super::Init();
	//objectpoolmanager ����
	RegisterManagerClass(UCPP_ObjectPoolManager::StaticClass());
}

void UCPP_MultiplayGameInstance::RegisterManagerClass(TSubclassOf<UCPP_UManagerClass> managerClass)
{
	UCPP_UManagerClass* managerClassInst = NewObject<UCPP_UManagerClass>(this,managerClass,NAME_None,EObjectFlags::RF_MarkAsRootSet);
	//������ �÷��Ͱ� ȣ����� �ʵ��� ����
	managerClassInst->InitManagerClass();
	//TMap�� ����
	ManagerClasses.Add(managerClass->GetName(), managerClassInst);
	UE_LOG(LogTemp,Display,L"%s",*managerClass->GetName());
}
