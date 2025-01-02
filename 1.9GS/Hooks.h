#pragma once
#include "framework.h"
#include "Util.h"
#include "PlayerController.h"

namespace Hooks
{
	std::unordered_map<std::string, void(*)(UObject*, void*)> FunctionHandlers = 
	{
		{ "ServerAcknowledgePossession", PlayerController::ServerAcknowledgePossession },
		{ "ServerLoadingScreenDropped", PlayerController::ServerLoadingScreenDropped }
	};

	static void* (*ProcessEvent)(UObject* Object, UFunction* Function, void* Parameters);
	static void* ProcessEventHook(UObject* Object, UFunction* Function, void* Parameters)
	{
		auto It = FunctionHandlers.find(Function->GetName());
		if (It != FunctionHandlers.end())
			It->second(Object, Parameters);
		return ProcessEvent(Object, Function, Parameters);
	}

	static void InitHook()
	{
		Util::CreateHook(Util::GetOffset(Offsets::ProcessEvent), ProcessEventHook, &ProcessEvent);
	}
}