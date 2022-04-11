#include "Projectile/CPP_ProjectileAP.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Tank/CPP_Tank_Pawn.h"

void ACPP_ProjectileAP::BounceCal(float hitAngle, EHitDir hitDir)
{//AP탄으로 도탄여부 계산  데미지가 1이면 도탄
	UE_LOG(LogTemp,Display,L"%.2f",hitAngle);
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

void ACPP_ProjectileAP::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(Cast<ACPP_Tank_Pawn>(OtherActor)&&Cast<UBoxComponent>(OtherComp)&&!IsOverlap)
	{
		//중복을 방지하기 위한 overlap 확인 변수
		IsOverlap = true;
		//포탄의 입사각 계산
		float HitAngle = GetHitAngle(OtherComp,SweepResult);
		
		UE_LOG(LogTemp,Display,L"angle %.2f",HitAngle);
		const UEnum* DirEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EHitDir"), true);
		FString EnumToString = DirEnum->GetNameStringByValue((int64)ProjectileHitDir);

		UE_LOG(LogTemp,Display,L"%s",*EnumToString);
		//도탄 판정
		BounceCal(HitAngle,ProjectileHitDir);
		//데미지 주기
		UGameplayStatics::ApplyPointDamage(OtherActor,Damage,SweepResult.Location,SweepResult,PlayerCtrl,this,nullptr);
		Disable();
	}
	//삭제	
	Super::OnBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

