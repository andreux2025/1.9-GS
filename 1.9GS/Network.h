#pragma once
#include "framework.h"
#include "Helpers.h"
#include "Util.h"

namespace Network
{
	static void StartServer()
	{
		AOnlineBeaconHost* Beacon = Helpers::SpawnActor<AOnlineBeaconHost>();
		Beacon->InitHost();
		Beacon->ListenPort = 7777;
		Beacon->NetDriverName = Helpers::KismetStringLibrary()->Conv_StringToName(L"GameNetDriver");
		Beacon->NetDriver->NetDriverName = Helpers::KismetStringLibrary()->Conv_StringToName(L"GameNetDriver");
		FString Error;
		FURL URL;
		URL.Port = Beacon->ListenPort;
		((UIpNetDriver*)Beacon->NetDriver)->InitListen(UWorld::GetWorld(), URL, false, Error);
		UWorld::GetWorld()->NetDriver = Beacon->NetDriver;
		Beacon->NetDriver->SetWorld(UWorld::GetWorld());
		UWorld::GetWorld()->LevelCollections[0].NetDriver = UWorld::GetWorld()->NetDriver;
		UWorld::GetWorld()->LevelCollections[1].NetDriver = UWorld::GetWorld()->NetDriver;
		LOG("Server Started On Port " << URL.Port << "!");
	}

	__int64 __fastcall NoReservation(__int64* a1, __int64 a2, char a3, __int64 a4)
	{
		return 0;
	}

	char __fastcall ValidationFailure(__int64* a1, __int64 a2)
	{
		return 0;
	}

	ENetMode GetNetMode(UWorld* InWorld)
	{
		return NM_DedicatedServer;
	}

	float GetMaxTickRate()
	{
		return 30.0f;
	}

	static void InitHook()
	{
		Util::CreateHook(Util::GetOffset(0x719F90), NoReservation);
		Util::CreateHook(Util::GetOffset(0x70F690), ValidationFailure);
		Util::CreateHook(Util::GetOffset(0x25468e0), GetNetMode);
		Util::CreateHook(Util::GetOffset(0x24f8b20), GetMaxTickRate);
	}
}