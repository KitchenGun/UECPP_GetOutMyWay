#include "Tank/Component/CPP_ParticleControlComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Particles/ParticleSystemComponent.h"

UCPP_ParticleControlComponent::UCPP_ParticleControlComponent()
{
	if(IsValid(GetOwner()))
		OwnerTank = Cast<ACPP_Tank_Pawn>(GetOwner());
}

void UCPP_ParticleControlComponent::BeginPlay()
{
	Super::BeginPlay();
	MuzzleFlashEffect = OwnerTank->GetMuzzleFlashEffect();
	ShockWaveEffect = OwnerTank->GetShockWaveEffect();
	WheelsEffect = OwnerTank->GetWheelsEffect();
}

bool UCPP_ParticleControlComponent::OnWheelParticle(bool IsMove)
{
	if(IsMove)
	{
		for(int i =0;i<WheelsEffect.Num();i++)
		{
			WheelsEffect[i]->SetActive(true);
		}
		return true;
	}
	else
	{
		for(int i =0;i<WheelsEffect.Num();i++)
		{
			WheelsEffect[i]->SetActive(false);
		}
		return false;
	}
}

void UCPP_ParticleControlComponent::OnFireParticle()
{
	//particle 사용
	MuzzleFlashEffect->Activate(true);
	ShockWaveEffect->Activate(true);
	FVector Start = MuzzleFlashEffect->GetComponentLocation();
	FVector End = MuzzleFlashEffect->GetComponentLocation();
	TArray<AActor*> ignore;
	TArray<FHitResult> HitResults;
	TArray<AActor*> ImpactArray;
	float blastRange = 1000;
	float ShockWaveForce=1e+2;
	//포 발사에 따른 충격파 구현
	const bool Hit =
		UKismetSystemLibrary::SphereTraceMulti(GetWorld(),Start,End,blastRange,
			ETraceTypeQuery::TraceTypeQuery8,false,ignore,EDrawDebugTrace::None,HitResults,true);
	if(Hit)
	{
		int32 index;
		for(FHitResult temp:HitResults)
		{
			AActor* tempActor=Cast<AActor>(temp.Actor);
			if(IsValid(tempActor))
			{
				ImpactArray.Find(tempActor,index);
				if(index==INDEX_NONE)
				{
					ImpactArray.Add(tempActor);
					UStaticMeshComponent* MeshComponent = Cast<UStaticMeshComponent>(tempActor->GetRootComponent());
					if(IsValid(MeshComponent)&&MeshComponent->BodyInstance.bSimulatePhysics)
					{
						MeshComponent->AddImpulse(FVector(MeshComponent->GetComponentLocation()-(Start-FVector(0,200,0)))*ShockWaveForce);
					}
				}
			}
		}
	}
	
}




