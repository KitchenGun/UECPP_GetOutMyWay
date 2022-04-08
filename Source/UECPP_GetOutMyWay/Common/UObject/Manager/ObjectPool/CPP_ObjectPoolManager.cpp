
#include "Common/UObject/Manager/ObjectPool/CPP_ObjectPoolManager.h"

ICPP_Objectpooling* UCPP_ObjectPoolManager::GetRecycledObject(int32 objId)
{
	for(auto obj : PoolObjects)
	{
		if(obj->GetID()==objId)
		{
			if(obj->GetCanRecycle(objId))
				return obj;
		}
	}
	return nullptr;
}

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

void UCPP_ObjectPoolManager::ShutdownManagerClass()
{
	
	Super::ShutdownManagerClass();
}
