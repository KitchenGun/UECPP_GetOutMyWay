#include "Tank/Component/CPP_TrackMovementComponent.h"
#include "GameFramework/Character.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"


UCPP_TrackMovementComponent::UCPP_TrackMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	for (int i = 0; i < BogieWheelCount; i++)
	{
		FWheelLocationData temp;//보기륜이 10개가 넘어가지 않는다고 가정하였음
		temp.BoneIndex = (i+2);
		Data.Push(temp);
	}
	for (int i = 0; i < BogieWheelCount; i++)
	{
		FWheelLocationData temp;//보기륜이 10개가 넘어가지 않는다고 가정하였음
		temp.BoneIndex = (i + 2);
		Data.Push(temp);
	}
}


void UCPP_TrackMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	Owner = Cast<APawn>(GetOwner());
	Mesh = Cast<USkeletalMeshComponent>(Owner->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
}


void UCPP_TrackMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (int i = 0; i < Data.Num(); i++)
	{
		float Distance;
		Trace(i, Distance);
		Data[i].Distance.Z = UKismetMathLibrary::FInterpTo(Data[i].Distance.Z, Distance, DeltaTime, InterpSpeed);
	}
}

void UCPP_TrackMovementComponent::Trace(int32 Index, float& OutDistance)
{
	FName TraceStartBoneName;
	FName TraceWheelBoneName;
	SetTraceBoneName(TraceStartBoneName, TraceWheelBoneName,Index);
	FVector socketLocation = Mesh->GetSocketLocation(TraceWheelBoneName);//wheel 의 xy좌표를 가져옴
	float z = Mesh->GetSocketLocation(TraceStartBoneName).Z;//추적 시작 Z좌표를 가져옴
	if (Index == 0 || Index == 7)
		z -= 12;
	FVector start = FVector(socketLocation.X, socketLocation.Y, z);
	//추적할 거리를 빼줘서 탐지 끝나는 거리 설정
	z = start.Z -TraceDistance;
	FVector end = FVector(socketLocation.X, socketLocation.Y, z);

	TArray<AActor*> ignoreActors;
	ignoreActors.Add(Owner);

	FHitResult hitResult;
	//linetrace 생성
	UKismetSystemLibrary::LineTraceSingle(GetWorld(), start, end, ETraceTypeQuery::TraceTypeQuery2, true,
		ignoreActors, EDrawDebugTrace::None, hitResult, true, FLinearColor::Green, FLinearColor::Red);

	//초기화
	OutDistance = 0;
	//OutRotation = FRotator::ZeroRotator;
	//충돌 없으면 반환
	if (!hitResult.bBlockingHit)
	{
		OutDistance = -10;//기본적인 추적 시작 위치와 wheel의 높이 차이
		return;
	}
	float length = (hitResult.ImpactPoint - hitResult.TraceEnd).Size();
	//거리 반환
	OutDistance = length-Offset;
}

void UCPP_TrackMovementComponent::SetTraceBoneName(FName& TraceStartBone, FName& TraceWheelBone, int32 Index)
{
	FString tempStartName;
	FString tempWheelName;

	if(Index < BogieWheelCount)
	{ 
		tempStartName = "lf_hidraulic_1_";
		tempStartName.AppendInt(Index+2);
		tempStartName.Append("_jnt");

		tempWheelName = "lf_wheel_0";
		tempWheelName.AppendInt(Index + 2);
		tempWheelName.Append("_track_jnt");
	}
	else
	{
		tempStartName = "rt_hidraulic_1_";
		tempStartName.AppendInt(Index + 2 - BogieWheelCount);
		tempStartName.Append("_jnt");

		tempWheelName = "rt_wheel_0";
		tempWheelName.AppendInt(Index + 2 - BogieWheelCount);
		tempWheelName.Append("_track_jnt");
	}
		
	TraceStartBone = FName(tempStartName);
	TraceWheelBone = FName(tempWheelName);
}

