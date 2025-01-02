#pragma once
#include "framework.h"
#include "Globals.h"

namespace Helpers
{
	static UKismetStringLibrary* KismetStringLibrary()
	{
		return ((UKismetStringLibrary*)UKismetStringLibrary::StaticClass()->GetDefaultObj());
	}

	static UKismetMathLibrary* KismetMathLibrary()
	{
		return ((UKismetMathLibrary*)UKismetMathLibrary::StaticClass()->GetDefaultObj());
	}

	static FQuat RotationToQuat(FRotator Rotation)
	{
		FQuat Quat;

		auto DEG_TO_RAD = 3.14159 / 180;
		auto DIVIDE_BY_2 = DEG_TO_RAD / 2;

		auto SP = sin(Rotation.Pitch * DIVIDE_BY_2);
		auto CP = cos(Rotation.Pitch * DIVIDE_BY_2);

		auto SY = sin(Rotation.Yaw * DIVIDE_BY_2);
		auto CY = cos(Rotation.Yaw * DIVIDE_BY_2);

		auto SR = sin(Rotation.Roll * DIVIDE_BY_2);
		auto CR = cos(Rotation.Roll * DIVIDE_BY_2);

		Quat.X = CR * SP * SY - SR * CP * CY;
		Quat.Y = -CR * SP * CY - SR * CP * SY;
		Quat.Z = CR * CP * SY - SR * SP * CY;
		Quat.W = CR * CP * CY + SR * SP * SY;

		return Quat;
	}

	template<typename T>
	static T* SpawnActor(FVector Location = { 0,0,0 }, FRotator Rotation = { 0,0,0 }, UClass* Class = T::StaticClass())
	{
		FTransform Transform;
		Transform.Rotation = RotationToQuat(Rotation);
		Transform.Translation = Location;
		Transform.Scale3D = { 1,1,1 };
		return (T*)Globals::GameplayStatics()->FinishSpawningActor(Globals::GameplayStatics()->BeginDeferredActorSpawnFromClass(UWorld::GetWorld(), Class, Transform, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn, nullptr), Transform);
	}
}