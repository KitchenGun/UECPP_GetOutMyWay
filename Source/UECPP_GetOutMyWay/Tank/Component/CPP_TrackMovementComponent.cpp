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
		FWheelLocationData temp;//������� 10���� �Ѿ�� �ʴ´ٰ� �����Ͽ���
		temp.BoneIndex = (i+2);
		Data.Push(temp);
	}
	for (int i = 0; i < BogieWheelCount; i++)
	{
		FWheelLocationData temp;//������� 10���� �Ѿ�� �ʴ´ٰ� �����Ͽ���
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
	FVector socketLocation = Mesh->GetSocketLocation(TraceWheelBoneName);//wheel �� xy��ǥ�� ������
	float z = Mesh->GetSocketLocation(TraceStartBoneName).Z;//���� ���� Z��ǥ�� ������
	//UE_LOG(LogTemp, Display, L"%f", z);
	FVector start = FVector(socketLocation.X, socketLocation.Y, z);
	//������ �Ÿ��� ���༭ Ž�� ������ �Ÿ� ����
	z = start.Z -TraceDistance;
	FVector end = FVector(socketLocation.X, socketLocation.Y, z);

	TArray<AActor*> ignoreActors;
	ignoreActors.Add(Owner);

	FHitResult hitResult;
	//linetrace ����
	UKismetSystemLibrary::LineTraceSingle(GetWorld(), start, end, ETraceTypeQuery::TraceTypeQuery3, true,
		ignoreActors, EDrawDebugTrace::ForOneFrame, hitResult, true, FLinearColor::Green, FLinearColor::Red);

	//�ʱ�ȭ
	OutDistance = 0;
	//OutRotation = FRotator::ZeroRotator;
	//�浹 ������ ��ȯ
	if (!hitResult.bBlockingHit)
	{
		if(Index==2)
			OutDistance = -75;
		else
			OutDistance = -65;//�⺻���� ���� ���� ��ġ�� wheel�� ���� ����
		return;
	}
	float length = (hitResult.ImpactPoint - hitResult.TraceEnd).Size();
	//�Ÿ� ��ȯ
	OutDistance = length;
	//if(BoneName == "lf_wheel_02_track_jnt")
		//UE_LOG(LogTemp, Display, L"%f", OutDistance);
	//DegAtan�� ��� ������ �޾Ƽ� ���̸� ��ȯ�Ѵ�
	//DegAtan2�� ��� b/a�� ���� ��ȯ�ؼ� ���̸� ��ȯ�Ѵ�
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

