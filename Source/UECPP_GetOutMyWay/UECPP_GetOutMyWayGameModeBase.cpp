#include "UECPP_GetOutMyWayGameModeBase.h"

#include "Blueprint/UserWidget.h"

AUECPP_GetOutMyWayGameModeBase::AUECPP_GetOutMyWayGameModeBase()
{
	//ConstructorHelpers::FClassFinder<ACPP_M1A1> player(L"Blueprint'/Game/BP/Tank/BP_M1A1.BP_M1A1_C'");//class는_C붙여야함
	ConstructorHelpers::FClassFinder<APawn> player(L"Blueprint'/Game/BP/Tank/BP_M1A1Pawn.BP_M1A1Pawn_C'");//testpawn 으로 설정중
	if (player.Succeeded())
	{
		DefaultPawnClass = player.Class;
	}
}
