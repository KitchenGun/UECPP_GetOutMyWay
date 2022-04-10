#include "Projectile/CPP_ProjectileAP.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Tank/CPP_Tank_Pawn.h"

void ACPP_ProjectileAP::BounceCal(float hitAngle, EHitDir hitDir)
{//AP탄으로 도탄여부 계산  데미지가 1이면 도탄 
	switch (ProjectileHitDir)
	{
	case EHitDir::Front:
		if(60<hitAngle&&hitAngle<120)//60도 미만의 입사각 == 도탄
			break;
		else
			Damage = 1;
		break;
	case EHitDir::Side:
		if(30<hitAngle&&hitAngle<150)//30도 미만의 입사각 == 도탄
			break;
		else
			Damage = 1;
		break;
	case EHitDir::Back:
		if(10<hitAngle&&hitAngle<170)//10도 미만의 입사각 == 도탄
			break;
		else
			Damage = 1;
		break;
	case EHitDir::UpSide:
		if(60<hitAngle&&hitAngle<120)//60도 미만의 입사각 == 도탄
			break;
		else
			Damage = 1;
		break;
	case  EHitDir::DownSide:
		if(45<hitAngle&&hitAngle<135)//45도 미만의 입사각 == 도탄
			break;
		else
			Damage = 1;
		break;
	default:
		break;
	}
}


void ACPP_ProjectileAP::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if(Cast<ACPP_Tank_Pawn>(OtherActor)&&Cast<UBoxComponent>(OtherComp))
	{
		//포탄의 입사각 계산
		float HitAngle = GetHitAngle(HitComponent,OtherComp,Hit);
	
		const UEnum* DirEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EHitDir"), true);
		FString EnumToString = DirEnum->GetNameStringByValue((int64)ProjectileHitDir);
	
		//도탄 판정
		BounceCal(HitAngle,ProjectileHitDir);
		//데미지 주기
		UGameplayStatics::ApplyPointDamage(OtherActor,Damage,Hit.Location,Hit,PlayerCtrl,this,nullptr);
		Disable();
	}
	//삭제	
	Super::OnHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
		
}
