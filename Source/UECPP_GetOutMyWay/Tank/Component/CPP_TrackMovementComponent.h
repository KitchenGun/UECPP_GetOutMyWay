#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CPP_TrackMovementComponent.generated.h"

USTRUCT(BlueprintType)
struct FWheelLocationData
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly,EditAnywhere,Category="Wheel")
	FVector Distance=FVector::ZeroVector;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Wheel")
	FName BoneName = "";
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
	UPROPERTY(EditAnywhere,Category = "Count")//����� ������ ���� ��ü����
	int BogieWheelCount = 7;//����� ����
	void GetData(TArray<FWheelLocationData> &AniData)
	{
		for (int i = 0; i < Data.Num(); i++)
		{
			AniData[i].BoneName = Data[i].BoneName;
			AniData[i].Distance = Data[i].Distance;
		}
	}
private:
	UPROPERTY(EditAnywhere,Category = "Trace")
	float InterpSpeed = 10;
	UPROPERTY(EditAnywhere, Category = "Trace")
	float TraceDistance = 40;
	UPROPERTY(EditAnywhere,Category ="Trace")
	float Offset = 35;

	void Trace(FName BoneName,float &OutDistance);

	class ACharacter* OwnerCharacter;

	TArray<FWheelLocationData> Data;
};
