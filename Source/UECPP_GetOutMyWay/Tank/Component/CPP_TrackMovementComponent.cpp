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
		FString tempName = "lf_wheel_0";
		tempName.AppendInt(i+2);
		tempName.Append("_track_jnt");
		temp.BoneName = FName(tempName);
		Data.Push(temp);
	}
	for (int i = 0; i < BogieWheelCount; i++)
	{
		FWheelLocationData temp;//������� 10���� �Ѿ�� �ʴ´ٰ� �����Ͽ���
		FString tempName = "rt_wheel_0";
		tempName.AppendInt(i+2);
		tempName.Append("_track_jnt");
		temp.BoneName = FName(tempName);
		//UE_LOG(LogTemp, Log, TEXT("Bone Name :: %s"), *temp.BoneName.ToString());
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
		Trace(Data[i].BoneName, Distance);
		UE_LOG(LogTemp,Display,L"%.2f",Distance);
		Data[i].Distance.Z = UKismetMathLibrary::FInterpTo(Data[i].Distance.Z, Distance+Offset, DeltaTime, InterpSpeed);
	}
}

void UCPP_TrackMovementComponent::Trace(FName BoneName, float& OutDistance)
{
/*
	float z = OwnerCharacter->GetActorLocation().Z;	
	FVector start = FVector(socketLocation.X, socketLocation.Y, z);
	//������ �Ÿ��� ���༭ Ž�� ������ �Ÿ� ����
	z = start.Z - (OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()) - TraceDistance;
	FVector end = FVector(socketLocation.X, socketLocation.Y, z);
*/
	FVector socketLocation = Mesh->GetSocketLocation(BoneName);
	float z = Owner->GetActorLocation().Z-50;
	FVector start = FVector(socketLocation.X, socketLocation.Y, z);
	//������ �Ÿ��� ���༭ Ž�� ������ �Ÿ� ����
	z = start.Z -TraceDistance;// 16 - TraceDistance;
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
		OutDistance = -75;
		return;
	}
	float length = (hitResult.ImpactPoint - hitResult.TraceEnd).Size();
	//�Ÿ� ��ȯ
	OutDistance = length - TraceDistance;
	//if(BoneName == "lf_wheel_02_track_jnt")
	//	UE_LOG(LogTemp, Display, L"%f", OutDistance);
	//DegAtan�� ��� ������ �޾Ƽ� ���̸� ��ȯ�Ѵ�
	//DegAtan2�� ��� b/a�� ���� ��ȯ�ؼ� ���̸� ��ȯ�Ѵ�
	//float roll = UKismetMathLibrary::DegAtan2(hitResult.ImpactNormal.Y, hitResult.ImpactNormal.Z);
	//float pitch = UKismetMathLibrary::DegAtan2(hitResult.ImpactNormal.X, hitResult.ImpactNormal.Z);

	//OutRotation = FRotator(pitch, 0, roll);

}

