#include "Projectile/CPP_Projectile.h"

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Tank/CPP_Tank_Pawn.h"

ACPP_Projectile::ACPP_Projectile()
{
	//����
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(L"Collider");
	Shell = CreateDefaultSubobject<UStaticMeshComponent>(L"Shell");
	WarHead = CreateDefaultSubobject<UStaticMeshComponent>(L"WarHead");
	Effect = CreateDefaultSubobject<UStaticMeshComponent>(L"Effect");
	
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(L"ProjectileMovement");

	//��ӱ��� ����
	Shell->SetupAttachment(Capsule);
	WarHead->SetupAttachment(Capsule);
	Effect->SetupAttachment(Capsule);
	//S*R*T
	Capsule->SetRelativeRotation(FRotator(90,0,0));
	Capsule->SetCapsuleHalfHeight(70);
	Capsule->SetCapsuleRadius(20);
	Shell->SetRelativeScale3D(FVector(10,10,10));
	Shell->SetRelativeRotation(FRotator(0,90,-90));
	Shell->SetRelativeLocation(FVector(0,0,20));
	WarHead->SetRelativeScale3D(FVector(0.05f,0.05f,0.05f));
	WarHead->SetRelativeRotation(FRotator(-90,0,0));
	WarHead->SetRelativeLocation(FVector(0,0,-20));
	Effect->SetRelativeScale3D(FVector(0.4f,0.1f,0.1f));
	Effect->SetRelativeRotation(FRotator(90,0,0));
	Effect->SetRelativeLocation(FVector(0,0,-45));
	//���� ����
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
	
	ProjectileMovement->InitialSpeed = 1e+4f;
	ProjectileMovement->MaxSpeed = 1e+4f;
	ProjectileMovement->ProjectileGravityScale = 0;
}

void ACPP_Projectile::BeginPlay()
{
	Super::BeginPlay();
	Capsule->OnComponentHit.AddDynamic(this, &ACPP_Projectile::OnHit);
	//capsule�� ȸ���Ǿ� �־ �̷��� �����ؼ� ����� -> -Capsule->GetUpVector()
	StartPos = this->GetActorLocation();
	ProjectileMovement->Velocity = -Capsule->GetUpVector()*ProjectileMovement->InitialSpeed;
	InitialLifeSpan = 5.0f;
}

float ACPP_Projectile::GetHitAngle(UPrimitiveComponent* HitComponent, UPrimitiveComponent* OtherComp,
	const FHitResult& Hit)
{
	//�浹�� �޽��� ũ�� ��ȯ
	FVector compScale =Cast<UBoxComponent>(OtherComp)->GetScaledBoxExtent();
	float x,y,z;
	x = compScale.X;
	y = compScale.Y;
	z = compScale.Z;
	//ũ�⸦ �̿��ؼ� �Ž��� ������ ���� �밢�� ������ ������ ���� ������ ����
	float h = sqrtf(y*y+z*z);
	float a = sqrtf(h*h+x*x);
	FrontSideJudgeAngle = asinf(h/a);
	FrontSideJudgeAngle = FMath::RadiansToDegrees(FrontSideJudgeAngle);
	//�浹 ���� ����
	FVector HitVec = Hit.Location-StartPos;
	HitVec =HitVec.GetSafeNormal();
	//�浹�� ������Ʈ�� ���� ����
	FVector HitObjVec = OtherComp->GetComponentRotation().Vector();
	HitObjVec = HitObjVec.GetSafeNormal();
	//�浹�� �ľ��ϱ�
	bool isFrontOrBack = false;
	
	

	//�κ����� ��Ÿ�� ���ؾ���
	float angle =FMath::Acos(FVector::DotProduct(HitVec,HitObjVec));
	angle = FMath::RadiansToDegrees(angle);
	//�浹���� �������� �������� Ȯ��
	if(angle<FrontSideJudgeAngle
		||angle>180-FrontSideJudgeAngle)
	{
		//�����̳� �ĸ��� ���
		HitObjVec = (OtherComp->GetComponentRotation()+FRotator(0,90.0f,0)).Vector();
		HitObjVec = HitObjVec.GetSafeNormal();
		//�翬��
		angle =FMath::Acos(FVector::DotProduct(HitVec,HitObjVec));
		angle = FMath::RadiansToDegrees(angle);
	}
	return angle;
}

void ACPP_Projectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                            FVector NormalImpulse, const FHitResult& Hit)
{//��� ���� ���� �浹�� ����� �ٸ��� ������ ������ ���� ź���� �����ҷ��� ��
	if(Cast<ACPP_Tank_Pawn>(OtherActor))
	{
		float HitAngle = GetHitAngle(HitComponent,OtherComp,Hit);
		UE_LOG(LogTemp,Display,L"HitAngle %.2f",HitAngle);
		UE_LOG(LogTemp,Display,L"FrontSideJudgeAngle %.2f",FrontSideJudgeAngle);
	}
	
	Destroy();
}


