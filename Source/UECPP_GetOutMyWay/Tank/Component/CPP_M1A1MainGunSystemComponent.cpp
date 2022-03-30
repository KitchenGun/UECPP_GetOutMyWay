
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
	//탄약 세팅//현재는 0만 사용할거임
	Ammunition.SetNum(4);//5개 탄종 넣을 공간 확보
	Ammunition[0] = (int32)20;
	Ammunition[1] = (int32)10;
	Ammunition[2] = (int32)10;
	
}

UCPP_M1A1MainGunSystemComponent::UCPP_M1A1MainGunSystemComponent()
{
	//레퍼런스 가져오기
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
	//재장전관련 메소드는 Super	
	Super::MainGunFire();
}
