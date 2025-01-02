#pragma once
#include "framework.h"

namespace PlayerController
{
	void ServerAcknowledgePossession(UObject* Object, void* Parameters)
	{
		APlayerController* PlayerController = (APlayerController*)Object;
		PlayerController->AcknowledgedPawn = PlayerController->Pawn;
	}

	static void (*ApplyCharacterCustomization)(AFortPlayerStateAthena* PlayerState, APlayerPawn_Athena_C* PlayerPawn) = decltype(ApplyCharacterCustomization)(Util::GetOffset(0x87a800));
	void ServerLoadingScreenDropped(UObject* Object, void* Parameters)
	{
		AFortPlayerControllerAthena* PlayerController = (AFortPlayerControllerAthena*)Object;
		APlayerPawn_Athena_C* PlayerPawn = (APlayerPawn_Athena_C*)PlayerController->Pawn;
		AFortPlayerStateAthena* PlayerState = (AFortPlayerStateAthena*)PlayerController->PlayerState;
		ApplyCharacterCustomization(PlayerState, PlayerPawn);
	}
}