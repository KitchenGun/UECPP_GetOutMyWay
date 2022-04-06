
#include "Common/UObject/Manager/ObjectPool/CPP_ObjectPoolManager.h"

void UCPP_ObjectPoolManager::BeginPlay()
{
	Super::BeginPlay();
	//저장되어있던 객체 초기화
	PoolObjects.Empty();
}

void UCPP_ObjectPoolManager::InitManagerClass()
{
	Super::InitManagerClass();
}
