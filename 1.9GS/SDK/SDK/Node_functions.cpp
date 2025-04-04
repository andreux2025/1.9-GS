#pragma once

/*
* SDK generated by Dumper-7
*
* https://github.com/Encryqed/Dumper-7
*/

// Package: Node

#include "Basic.hpp"

#include "Node_classes.hpp"
#include "Node_parameters.hpp"


namespace SDK
{

// Function Node.Node_C.ExecuteUbergraph_Node
// ()
// Parameters:
// int32                                   EntryPoint                                             (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash)

void UNode_C::ExecuteUbergraph_Node(int32 EntryPoint)
{
	static class UFunction* Func = nullptr;

	if (Func == nullptr)
		Func = Class->GetFunction("Node_C", "ExecuteUbergraph_Node");

	Params::Node_C_ExecuteUbergraph_Node Parms{};

	Parms.EntryPoint = EntryPoint;

	UObject::ProcessEvent(Func, &Parms);
}


// Function Node.Node_C.Construct
// (BlueprintCosmetic, Event, Public, BlueprintEvent)

void UNode_C::Construct()
{
	static class UFunction* Func = nullptr;

	if (Func == nullptr)
		Func = Class->GetFunction("Node_C", "Construct");

	UObject::ProcessEvent(Func, nullptr);
}


// Function Node.Node_C.PreConstruct
// (BlueprintCosmetic, Event, Public, BlueprintEvent)
// Parameters:
// bool                                    IsDesignTime                                           (Parm, ZeroConstructor, IsPlainOldData, NoDestructor)

void UNode_C::PreConstruct(bool IsDesignTime)
{
	static class UFunction* Func = nullptr;

	if (Func == nullptr)
		Func = Class->GetFunction("Node_C", "PreConstruct");

	Params::Node_C_PreConstruct Parms{};

	Parms.IsDesignTime = IsDesignTime;

	UObject::ProcessEvent(Func, &Parms);
}


// Function Node.Node_C.SetHighContrastMode
// (Public, BlueprintCallable, BlueprintEvent)
// Parameters:
// bool                                    ActivateHighContrast                                   (Parm, ZeroConstructor, IsPlainOldData, NoDestructor)

void UNode_C::SetHighContrastMode(bool ActivateHighContrast)
{
	static class UFunction* Func = nullptr;

	if (Func == nullptr)
		Func = Class->GetFunction("Node_C", "SetHighContrastMode");

	Params::Node_C_SetHighContrastMode Parms{};

	Parms.ActivateHighContrast = ActivateHighContrast;

	UObject::ProcessEvent(Func, &Parms);
}

}

