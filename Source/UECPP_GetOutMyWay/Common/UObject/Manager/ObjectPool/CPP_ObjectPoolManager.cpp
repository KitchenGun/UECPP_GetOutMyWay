
#include "Common/UObject/Manager/ObjectPool/CPP_ObjectPoolManager.h"

void UCPP_ObjectPoolManager::BeginPlay()
{
	Super::BeginPlay();
	//����Ǿ��ִ� ��ü �ʱ�ȭ
	PoolObjects.Empty();
}

void UCPP_ObjectPoolManager::InitManagerClass()
{
	Super::InitManagerClass();
}
