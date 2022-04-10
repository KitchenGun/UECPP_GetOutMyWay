#include "Projectile/CPP_Projectile.h"

#include "DrawDebugHelpers.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Tank/CPP_Tank_Pawn.h"

ACPP_Projectile::ACPP_Projectile()
{
	//생성
	Root = CreateDefaultSubobject<USceneComponent>(L"Root");
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(L"Collider");
	Shell = CreateDefaultSubobject<UStaticMeshComponent>(L"Shell");
	WarHead = CreateDefaultSubobject<UStaticMeshComponent>(L"WarHead");
	Effect = CreateDefaultSubobject<UStaticMeshComponent>(L"Effect");
	
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(L"ProjectileMovement");

	//상속구조 정리
	SetRootComponent(Root);
	Capsule->SetupAttachment(Root);
	Shell->SetupAttachment(Capsule);
	WarHead->SetupAttachment(Capsule);
	Effect->SetupAttachment(Capsule);
	//S*R*T
	Capsule->SetRelativeRotation(FRotator(-90,0,0));
	Capsule->SetCapsuleHalfHeight(70);
	Capsule->SetCapsuleRadius(20);
	Shell->SetRelativeScale3D(FVector(10,10,10));
	Shell->SetRelativeRotation(FRotator(0,90,90));
	Shell->SetRelativeLocation(FVector(0,0,-10));
	WarHead->SetRelativeScale3D(FVector(0.05f,0.05f,0.05f));
	WarHead->SetRelativeRotation(FRotator(90,180,0));
	WarHead->SetRelativeLocation(FVector(0,0,30));
	Effect->SetRelativeScale3D(FVector(0.4f,0.1f,0.1f));
	Effect->SetRelativeRotation(FRotator(-90,0,0));
	Effect->SetRelativeLocation(FVector(0,0,45));
	//세부 설정
	Capsule->BodyInstance.SetCollisionProfileName("BlockAll");
	Capsule->SetNotifyRigidBodyCollision(true);
	ConstructorHelpers::FObjectFinder<UStaticMesh> shellMesh (L"StaticMesh'/Game/VigilanteContent/Shared/Particles/StaticMeshes/SM_BulletShell_01.SM_BulletShell_01'");
	Shell->SetStaticMesh(shellMesh.Object);
	Shell->BodyInstance.SetCollisionProfileName("NoCollision");
	ConstructorHelpers::FObjectFinder<UStaticMesh> warheadMesh(L"StaticMesh'/Game/VigilanteContent/Shared/Particles/StaticMeshes/SM_RocketBooster_02_SM.SM_RocketBooster_02_SM'");
	WarHead->SetStaticMesh(warheadMesh.Object);
	WarHead->BodyInstance.SetCollisionProfileName("NoCollision");
	ConstructorHelpers::FObjectFinder<UMaterial> warheadMat(L"Material'/Game/VigilanteContent/Shared/Levels/Platform/LOGO/Materials/M_Vigilante_Logo.M_Vigilante_Logo'");
	WarHead->SetMaterial(0,warheadMat.Object);
	ConstructorHelpers::FObjectFinder<UStaticMesh> effectMesh(L"StaticMesh'/Game/VigilanteContent/Shared/Particles/StaticMeshes/SM_RocketBooster_03_SM.SM_RocketBooster_03_SM'");
	Effect->SetStaticMesh(effectMesh.Object);
	Effect->BodyInstance.SetCollisionProfileName("NoCollision");
	ProjectileMovement->InitialSpeed = 5e+3f;
	ProjectileMovement->MaxSpeed = 5e+3f;
	ProjectileMovement->ProjectileGravityScale = 0;
	
}

int32 ACPP_Projectile::GetID() const
{
	return ObjectPoolID;
}

void ACPP_Projectile::SetID(int32 id)
{
	ObjectPoolID = id;
}

bool ACPP_Projectile::GetCanRecycle(int32 id) const
{
	//충돌체와 매쉬가 활성화 중이면 아래와 false 반환
	return IsCanRecycle;
}

void ACPP_Projectile::SetCanRecycle(bool value)
{
	IsCanRecycle = value;
}

void ACPP_Projectile::OnRecycleStart()
{
	//상태 킴
	Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Shell->SetVisibility(true);
	WarHead->SetVisibility(true);
	Effect->SetVisibility(true);
	SetCanRecycle(false);
	//capsule이 회전되어 있어서 이렇게 변경해서 사용함 -> -Capsule->GetUpVector()
	ProjectileMovement->Velocity = Capsule->GetUpVector()*ProjectileMovement->InitialSpeed;
	StartPos = this->GetActorLocation();
	InitialLifeSpan = 5.0f;
	ProjectileMovement->SetComponentTickEnabled(true);
}

void ACPP_Projectile::OnRecycleStart(FVector pos, FRotator dir)
{
	FTransform Transform;
	Transform.SetLocation(pos);
	Transform.SetRotation(FQuat(dir));
	SetActorTransform(Transform);
	OnRecycleStart();
}


void ACPP_Projectile::Disable()
{
	//상태 정지
	Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Shell->SetVisibility(false);
	WarHead->SetVisibility(false);
	Effect->SetVisibility(false);
	ProjectileMovement->SetComponentTickEnabled(false);
	SetCanRecycle(true);
}

void ACPP_Projectile::BeginPlay()
{
	Super::BeginPlay();
	Capsule->OnComponentHit.AddDynamic(this, &ACPP_Projectile::OnHit);
	//capsule이 회전되어 있어서 이렇게 변경해서 사용함 -> -Capsule->GetUpVector()
	StartPos = this->GetActorLocation();
	ProjectileMovement->Velocity = Capsule->GetUpVector()*ProjectileMovement->InitialSpeed;
	InitialLifeSpan = 5.0f;
}

float ACPP_Projectile::GetHitAngle(UPrimitiveComponent* HitComponent, UPrimitiveComponent* OtherComp,
	const FHitResult& Hit)
{
	//이름
	UE_LOG(LogTemp,Display,L"%s",*OtherComp->GetName());
	//debug 용
	DrawDebugSphere(GetWorld(),Hit.ImpactPoint,50,20,FColor::Yellow,true,1,0,2);
	//맞은 부위 판별용
	FVector compScale =Cast<UBoxComponent>(OtherComp)->GetScaledBoxExtent();
	//InverseTransform
	FVector HitPosInverseTransform =UKismetMathLibrary::InverseTransformLocation(OtherComp->GetComponentTransform(),Hit.ImpactPoint);


	//충돌된 컴포넌트의 방향 벡터
	FVector HitObjVec = FVector::ZeroVector;
	
	//충돌 면 파악
	if(FMath::IsNearlyEqual(abs(HitPosInverseTransform.Y),abs(compScale.Y),0.1f))
	{
		ProjectileHitDir = EHitDir::Side;
		HitObjVec = OtherComp->GetComponentRotation().Vector();
	}
	else if(FMath::IsNearlyEqual(HitPosInverseTransform.X,compScale.X))
	{
		ProjectileHitDir = EHitDir::Front;
		HitObjVec = (OtherComp->GetComponentRotation()+FRotator(0,90.0f,0)).Vector();
	}
	else if(FMath::IsNearlyEqual(HitPosInverseTransform.X,-compScale.X))
	{
		ProjectileHitDir = EHitDir::Back;
		HitObjVec = (OtherComp->GetComponentRotation()+FRotator(0,180.0f,0)).Vector();
	}
	else if(FMath::IsNearlyEqual(HitPosInverseTransform.Z,compScale.Z))
	{
		ProjectileHitDir = EHitDir::UpSide;
		HitObjVec = (OtherComp->GetComponentRotation()+FRotator(90.0f,0,0)).Vector();
	}
	else 
	{
		ProjectileHitDir = EHitDir::Back;
		HitObjVec = (OtherComp->GetComponentRotation()+FRotator(-90.0f,0,0)).Vector();
	}
	//충돌된 컴포넌트의 방향벡터 정규화
	HitObjVec = HitObjVec.GetSafeNormal();


	//충돌 방향 벡터
	FVector HitVec = Hit.Location-StartPos;
	HitVec =HitVec.GetSafeNormal();

	//두벡터의 세타를 구해야함
	float angle =FMath::Acos(FVector::DotProduct(HitVec,HitObjVec));
	angle = FMath::RadiansToDegrees(angle);

	return angle;
}

void ACPP_Projectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                            FVector NormalImpulse, const FHitResult& Hit)
{//상속 받은 다음 충돌시 결과를 다르게 보내는 것으로 여러 탄종을 구현할려고 함
	Disable();
}


