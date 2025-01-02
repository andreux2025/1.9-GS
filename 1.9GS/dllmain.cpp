#include "framework.h"
#include "Util.h"
#include "Globals.h"
#include "Helpers.h"
#include "Hooks.h"
#include "GameMode.h"
#include "Network.h"
#include "NetworkDriver.h"

DWORD MainThread(HMODULE Module)
{
    MH_Initialize();
    Util::InitConsole();
    Globals::InitUEConsole();
    Hooks::InitHook();
    GameMode::InitHook();
    Network::InitHook();
    NetworkDriver::InitHook();
    LOG("Setting Up 1.9");
    Sleep(1500);
    Globals::GameplayStatics()->OpenLevel(UWorld::GetWorld(), Helpers::KismetStringLibrary()->Conv_StringToName(L"Athena_Terrain"), true, FString());
    for (int i = UWorld::GetWorld()->OwningGameInstance->LocalPlayers.Num() - 1; i >= 0; --i)
        UWorld::GetWorld()->OwningGameInstance->LocalPlayers.Remove(i);
    LOG("Credits: @TimmieDevelops");
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MainThread, hModule, 0, 0);
        break;
    }

    return TRUE;
}