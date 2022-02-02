#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CPP_TrackMovementComponent.generated.h"

USTRUCT(BlueprintType)
struct FWheelLocationData
{
	GENERATED_BODY()
	
	FWheelLocationData(int32 index = 0, FVector distance = FVector::ZeroVector)
	{
		Distance = distance;
		BoneIndex = index;
	}

public:
	UPROPERTY(BlueprintReadOnly,EditAnywhere,Category="Wheel")
	FVector Distance=FVector::ZeroVector;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Wheel")
	int32 BoneIndex = 0;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UECPP_GETOUTMYWAY_API UCPP_TrackMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCPP_TrackMovementComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(EditAnywhere,Category = "Count")//장비의 종류에 따라서 교체가능
	int BogieWheelCount = 7;//보기륜 갯수
	void GetData(TArray<FWheelLocationData> &AniData)
	{
		for (int i = 0; i < Data.Num(); i++)
		{
			AniData[i].BoneIndex = Data[i].BoneIndex;
			AniData[i].Distance = Data[i].Distance;
		}
	}

private:
	UPROPERTY(EditAnywhere,Category = "Trace")
	float InterpSpeed = 10;
	UPROPERTY(EditAnywhere, Category = "Trace")
	float TraceDistance = 75;
	UPROPERTY(EditAnywhere,Category ="Trace")
	float Offset = 15;

	void Trace(int32 BoneIndex,float &OutDistance);
	void SetTraceBoneName(FName &TraceStartBone, FName& TraceWheelBone, int32 Index);

	class APawn* Owner;
	class USkeletalMeshComponent* Mesh;

	TArray<FWheelLocationData> Data;
};
