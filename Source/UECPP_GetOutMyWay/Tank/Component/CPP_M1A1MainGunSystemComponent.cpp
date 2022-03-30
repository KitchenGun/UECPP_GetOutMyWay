
#include "Tank/Component/CPP_M1A1MainGunSystemComponent.h"

#include "Camera/CameraComponent.h"
#include "Projectile/CPP_Projectile.h"
#include "Tank/CPP_Tank_Pawn.h"

void UCPP_M1A1MainGunSystemComponent::BeginPlay()
{
	Super::BeginPlay();
	Owner=Cast<ACPP_Tank_Pawn>(GetOwner());
	if(IsValid(Owner))
	{
		Owner->FireFunc.BindUFunction(this,"MainGunFire");
	}
	//ź�� ����//����� 0�� ����Ұ���
	Ammunition.SetNum(4);//5�� ź�� ���� ���� Ȯ��
	Ammunition[0] = (int32)20;
	Ammunition[1] = (int32)10;
	Ammunition[2] = (int32)10;
	
}

UCPP_M1A1MainGunSystemComponent::UCPP_M1A1MainGunSystemComponent()
{
	//���۷��� ��������
	ProjectileClass =ConstructorHelpers::FClassFinder<ACPP_Projectile>
		(L"Blueprint'/Game/BP/Projectile/BP_Projectile.BP_Projectile_C'").Class;
	ReloadTime =M1A1ReloadTime;
}

void UCPP_M1A1MainGunSystemComponent::MainGunFire()
{
	if(IsMainGunCanFire)
	{
		if(ProjectileClass)
		{
			FVector SpawnPos	= TankMesh->GetSocketLocation("gun_1_jntSocket");
			FRotator Direction   = FRotator(TankMesh->GetSocketRotation("gun_1_jntSocket").Pitch,TankMesh->GetSocketRotation("gun_1_jntSocket").Yaw,0);

			ACPP_Projectile* temp = GetWorld()->SpawnActor<ACPP_Projectile>(ProjectileClass,SpawnPos,Direction);
			temp->SetEventInstigator(FString(GetOwner()->GetName()),Owner->GetController());
			if(FireEffectFunc.IsBound())
				FireEffectFunc.Execute();
		}
	}
	//���������� �޼ҵ�� Super	
	Super::MainGunFire();
}
