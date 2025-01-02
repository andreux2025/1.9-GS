#pragma once
#include "framework.h"

namespace Globals
{
	float Duration = 9999.0f;
	float EarlyDuration = 9999.0f;
	bool bStartServer = false;

	static UGameplayStatics* GameplayStatics()
	{
		return ((UGameplayStatics*)UGameplayStatics::StaticClass()->GetDefaultObj());
	}

	static void InitUEConsole()
	{
		UConsole* NewConsole = (UConsole*)GameplayStatics()->SpawnObject(UEngine::GetEngine()->ConsoleClass, UEngine::GetEngine()->GameViewport);
		UEngine::GetEngine()->GameViewport->ViewportConsole = NewConsole;
	}

	template<typename T>
	static T* StaticLoadObject(std::string Path, UClass* InClass = T::StaticClass(), UObject* Outer = nullptr)
	{
		UObject* (__fastcall * _StaticLoadObject)(UClass * Class, UObject * InOuter, const TCHAR * Name, const TCHAR * Filename, uint32_t LoadFlags, UObject * Sandbox, bool bAllowObjectReconciliation) = decltype(_StaticLoadObject)(Util::GetOffset(0x1b528f0));
		return (T*)_StaticLoadObject(InClass, Outer, std::wstring(Path.begin(), Path.end()).c_str(), nullptr, 0, nullptr, false);
	}
}