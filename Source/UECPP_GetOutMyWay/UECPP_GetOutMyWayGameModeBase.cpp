#include "Tank/CPP_M1A1.h"

#include "UECPP_GetOutMyWayGameModeBase.h"

AUECPP_GetOutMyWayGameModeBase::AUECPP_GetOutMyWayGameModeBase()
{
	ConstructorHelpers::FClassFinder<ACPP_M1A1> player(L"Blueprint'/Game/BP/Tank/BP_M1A1.BP_M1A1_C'");//class��_C�ٿ�����
	//ConstructorHelpers::FClassFinder<APawn> player(L"Blueprint'/Game/BP/Tank/TestVehiclePawn.TestVehiclePawn_C'");//tank test vehicle pawn ���� ������
	if (player.Succeeded())
	{
		DefaultPawnClass = player.Class;
	}
}