#pragma once
#include "framework.h"

namespace Util
{
	static void InitConsole()
	{
		AllocConsole();
		FILE* Dummy;
		freopen_s(&Dummy, "CONOUT$", "w", stdout);
		freopen_s(&Dummy, "CONIN$", "r", stdin);
		SetConsoleTitleW(L"1.9GS");
	}

	static uintptr_t GetOffset(DWORD Offset)
	{
		return (uintptr_t)GetModuleHandleW(nullptr) + Offset;
	}

	static void CreateHook(uintptr_t Offset, void* Detour, void* Original = nullptr)
	{
		MH_CreateHook(reinterpret_cast<LPVOID>(Offset), Detour, reinterpret_cast<LPVOID*>(Original));
		MH_EnableHook(reinterpret_cast<LPVOID>(Offset));
	}
}