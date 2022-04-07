#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Common/Interface/CPP_Objectpooling.h"
#include "Common/UObject/Manager/CPP_UManagerClass.h"
#include "CPP_ObjectPoolManager.generated.h"

UCLASS()
class UECPP_GETOUTMYWAY_API UCPP_ObjectPoolManager : public UCPP_UManagerClass
{
	GENERATED_BODY()
private :
	// 풀링할 오브젝트들을 저장할 배열
	TArray<ICPP_Objectpooling*> PoolObjects;
public :
	// 새로운 오브젝트를 등록합니다. 
	template<typename T>
	T* RegisterRecyclableObject(T* newRecyclableObject)
	{
		PoolObjects.Add(static_cast<ICPP_Objectpooling*>(newRecyclableObject));
		return newRecyclableObject;
	}

	// 재사용된 객체를 얻습니다.
	ICPP_Objectpooling* GetRecycledObject(int32 objId);

	//초기화
	virtual void BeginPlay() override;
	virtual void InitManagerClass() override;
	virtual void ShutdownManagerClass() override;
};
