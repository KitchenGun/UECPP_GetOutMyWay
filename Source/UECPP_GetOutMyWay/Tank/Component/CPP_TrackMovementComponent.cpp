#include "Tank/Component/CPP_TrackMovementComponent.h"
#include "GameFramework/Character.h"
#include "Components/ActorComponent.h"
#include "Components/CapsuleComponent.h"
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
	UE_LOG(LogTemp, Display, L"%d", Index);
	FVector socketLocation = Mesh->GetSocketLocation(TraceWheelBoneName);//wheel 의 xy좌표를 가져옴
	float z = Mesh->GetSocketLocation(TraceStartBoneName).Z;//추적 시작 Z좌표를 가져옴
	//UE_LOG(LogTemp, Display, L"%f", z);
	FVector start = FVector(socketLocation.X, socketLocation.Y, z);
	//추적할 거리를 빼줘서 탐지 끝나는 거리 설정
	z = start.Z -TraceDistance;
	FVector end = FVector(socketLocation.X, socketLocation.Y, z);

	TArray<AActor*> ignoreActors;
	ignoreActors.Add(Owner);

	FHitResult hitResult;
	//linetrace 생성
	UKismetSystemLibrary::LineTraceSingle(GetWorld(), start, end, ETraceTypeQuery::TraceTypeQuery3, true,
		ignoreActors, EDrawDebugTrace::ForOneFrame, hitResult, true, FLinearColor::Green, FLinearColor::Red);

	//초기화
	OutDistance = 0;
	//OutRotation = FRotator::ZeroRotator;
	//충돌 없으면 반환
	if (!hitResult.bBlockingHit)
	{
		if(Index==2)
			OutDistance = -75;
		else
			OutDistance = -65;//기본적인 추적 시작 위치와 wheel의 높이 차이
		return;
	}
	float length = (hitResult.ImpactPoint - hitResult.TraceEnd).Size();
	//거리 반환
	OutDistance = length;
	//if(BoneName == "lf_wheel_02_track_jnt")
		//UE_LOG(LogTemp, Display, L"%f", OutDistance);
	//DegAtan의 경우 각도를 받아서 길이를 반환한다
	//DegAtan2의 경우 b/a를 각각 반환해서 길이를 반환한다
	//float roll = UKismetMathLibrary::DegAtan2(hitResult.ImpactNormal.Y, hitResult.ImpactNormal.Z);
	//float pitch = UKismetMathLibrary::DegAtan2(hitResult.ImpactNormal.X, hitResult.ImpactNormal.Z);

	//OutRotation = FRotator(pitch, 0, roll);

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

