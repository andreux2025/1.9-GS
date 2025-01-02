#pragma once
#include "framework.h"
#include "Util.h"
#include "Globals.h"
#include "Network.h"
#include "NetworkDriver.h"

namespace GameMode
{
	APawn* SpawnDefaultPawnFor(AGameModeBase* GameMode, AController* NewPlayer, AActor* StartSpot)
	{
		FTransform Transform = StartSpot->GetTransform();
		Transform.Translation = StartSpot->K2_GetActorLocation();
		return GameMode->SpawnDefaultPawnAtTransform(NewPlayer, Transform);
	}

	bool ReadyToStartMatch(AFortGameModeAthena* GameMode)
	{
		AFortGameStateAthena* GameState = (AFortGameStateAthena*)GameMode->GameState;
		float TimeSeconds = Globals::GameplayStatics()->GetTimeSeconds(UWorld::GetWorld());
		GameMode->bWorldIsReady = true;
		GameState->WarmupCountdownEndTime = TimeSeconds + Globals::Duration;
		GameMode->WarmupCountdownDuration = Globals::Duration;
		GameState->WarmupCountdownStartTime = TimeSeconds;
		GameMode->WarmupEarlyCountdownDuration = Globals::EarlyDuration;
		GameState->GamePhase = EAthenaGamePhase::Warmup;
		GameState->OnRep_GamePhase(EAthenaGamePhase::Aircraft);
		if (!Globals::bStartServer)
			Network::StartServer();
		Globals::bStartServer = true;
		return true;
	}

	void HandleStartingNewPlayer(AGameModeBase* GameMode, AAthena_PlayerController_C* NewPlayer)
	{
		AAthena_GameState_C* GameState = (AAthena_GameState_C*)GameMode->GameState;
		APlayerPawn_Athena_C* NewPawn = Helpers::SpawnActor<APlayerPawn_Athena_C>(NewPlayer->K2_GetActorLocation(), NewPlayer->K2_GetActorRotation());
		AFortPlayerStateAthena* PlayerState = (AFortPlayerStateAthena*)NewPlayer->PlayerState;

		NewPlayer->Pawn = NewPawn;
		NewPlayer->OnRep_Pawn();
		NewPawn->Owner = NewPlayer;
		NewPawn->OnRep_Owner();
		NewPlayer->Possess(NewPawn);
		
		NewPawn->ForceNetUpdate();
		NewPlayer->ForceNetUpdate();

		NewPlayer->bHasClientFinishedLoading = true;
		NewPlayer->bHasServerFinishedLoading = true;
		NewPlayer->OnRep_bHasServerFinishedLoading();
		NewPlayer->bHasInitiallySpawned = true;

		PlayerState->bHasFinishedLoading = true;
		PlayerState->bHasStartedPlaying = true;
		PlayerState->OnRep_bHasStartedPlaying();

		NewPawn->SetMaxHealth(100);
		NewPawn->SetHealth(100);

		NewPawn->bCanBeDamaged = false;
		NewPawn->bReplicateMovement = true;
		NewPawn->OnRep_ReplicateMovement();

		NewPlayer->QuickBars = Helpers::SpawnActor<AFortQuickBars>();
		NewPlayer->QuickBars->SetOwner(NewPlayer);
		NewPlayer->WorldInventory = Helpers::SpawnActor<AFortInventory>();
		NewPlayer->WorldInventory->SetOwner(NewPlayer);
	}

	static void InitHook()
	{
		Util::CreateHook(Util::GetOffset(0x437dd0), SpawnDefaultPawnFor);
		Util::CreateHook(Util::GetOffset(0x432ae0), ReadyToStartMatch);
		Util::CreateHook(Util::GetOffset(0x5b9480), HandleStartingNewPlayer);
	}
}