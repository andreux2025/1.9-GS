#pragma once
#include "framework.h"
#include "Util.h"
#include "Globals.h"

namespace NetworkDriver
{
	static UActorChannel* FindChannel(UNetConnection* Connection, AActor* Actor)
	{
		for (int32 i = 0; i < Connection->OpenChannels.Num(); i++)
		{
			UActorChannel* Channel = (UActorChannel*)Connection->OpenChannels[i];
			if (Channel && Channel->Actor == Actor)
				return Channel;
		}

		return nullptr;
	}

	static __forceinline bool IsActorRelevantToConnection(AActor* Actor, TArray<FNetViewer>& ConnectionViewers)
	{
		for (int32 viewerIdx = 0; viewerIdx < ConnectionViewers.Num(); viewerIdx++)
		{
			if (Actor->IsNetRelevantFor(ConnectionViewers[viewerIdx].InViewer, ConnectionViewers[viewerIdx].ViewTarget, ConnectionViewers[viewerIdx].ViewLocation))
			{
				return true;
			}
		}

		return false;
	}

	// Returns true if this actor is owned by, and should replicate to *any* of the passed in connections
	static __forceinline UNetConnection* IsActorOwnedByAndRelevantToConnection(AActor* Actor, TArray<FNetViewer>& ConnectionViewers, bool& bOutHasNullViewTarget)
	{
		AActor* ActorOwner = Actor->Owner;

		bOutHasNullViewTarget = false;

		for (int i = 0; i < ConnectionViewers.Num(); i++)
		{
			UNetConnection* ViewerConnection = ConnectionViewers[i].Connection;

			if (ViewerConnection->ViewTarget == nullptr)
			{
				bOutHasNullViewTarget = true;
			}

			if (ActorOwner == ViewerConnection->PlayerController ||(ViewerConnection->PlayerController && ActorOwner == ViewerConnection->PlayerController->Pawn) || (ViewerConnection->ViewTarget && ViewerConnection->ViewTarget->IsRelevancyOwnerFor(Actor, ActorOwner, ViewerConnection->OwningActor)))
			{
				return ViewerConnection;
			}
		}

		return nullptr;
	}

	int32 ServerReplicateActors_PrepConnections(UNetDriver* NetDriver, float DeltaSeconds)
	{
		int32 NumClientsToTick = NetDriver->ClientConnections.Num();

		bool bFoundReadyConnection = false;

		for (int32 ConnIdx = 0; ConnIdx < NetDriver->ClientConnections.Num(); ConnIdx++)
		{
			UNetConnection* Connection = NetDriver->ClientConnections[ConnIdx];
			if (!Connection)
				continue;

			AActor* OwningActor = Connection->OwningActor;
			if (OwningActor)
			{
				bFoundReadyConnection = true;

				Connection->ViewTarget = Connection->PlayerController ? Connection->PlayerController->GetViewTarget() : OwningActor;

				for (int32 ChildIdx = 0; ChildIdx < Connection->Children.Num(); ChildIdx++)
				{
					UNetConnection* Child = Connection->Children[ChildIdx];
					APlayerController* ChildPlayerController = Child->PlayerController;
					if (ChildPlayerController)
					{
						Child->ViewTarget = ChildPlayerController->GetViewTarget();
					}
					else
					{
						Child->ViewTarget = nullptr;
					}
				}
			}
			else
			{
				Connection->ViewTarget = nullptr;
				for (int32 ChildIdx = 0; ChildIdx < Connection->Children.Num(); ChildIdx++)
				{
					Connection->Children[ChildIdx]->ViewTarget = nullptr;
				}
			}
		}

		return bFoundReadyConnection ? NumClientsToTick : 0;
	}

	void ServerReplicateActors_BuildConsiderList(UNetDriver* NetDriver, TArray<FNetworkObjectInfo*>& OutConsiderList)
	{
		for (TSharedPtr<FNetworkObjectInfo>& ObjectInfo : NetDriver->GetNetworkObjectList().GetActiveObjects())
		{
			FNetworkObjectInfo* ActorInfo = ObjectInfo.Get();

			if (!ActorInfo->bPendingNetUpdate && Globals::GameplayStatics()->GetTimeSeconds(UWorld::GetWorld()) <= ActorInfo->NextUpdateTime)
				continue;

			AActor* Actor = ActorInfo->Actor;

			if (Actor->GetRemoteRole() == ENetRole::ROLE_None)
				continue;

			if (Actor->NetDriverName != NetDriver->NetDriverName)
				continue;

			if (Actor->NetDormancy == ENetDormancy::DORM_Initial && Actor->bNetStartup)
				continue;

			if (ActorInfo->LastNetReplicateTime == 0)
			{
				ActorInfo->LastNetReplicateTime = Globals::GameplayStatics()->GetTimeSeconds(UWorld::GetWorld());
				ActorInfo->OptimalNetUpdateDelta = 1.0f / Actor->NetUpdateFrequency;
			}

			const float ScaleDownStartTime = 2.0f;
			const float ScaleDownTimeRange = 5.0f;

			const float LastReplicateDelta = Globals::GameplayStatics()->GetTimeSeconds(UWorld::GetWorld()) - ActorInfo->LastNetReplicateTime;

			if (LastReplicateDelta > ScaleDownStartTime)
			{
				if (Actor->MinNetUpdateFrequency == 0.0f)
				{
					Actor->MinNetUpdateFrequency = 2.0f;
				}

				// Calculate min delta (max rate actor will update), and max delta (slowest rate actor will update)
				const float MinOptimalDelta = 1.0f / Actor->NetUpdateFrequency;									  // Don't go faster than NetUpdateFrequency
				const float MaxOptimalDelta = Helpers::KismetMathLibrary()->Max(1.0f / Actor->MinNetUpdateFrequency, MinOptimalDelta); // Don't go slower than MinNetUpdateFrequency (or NetUpdateFrequency if it's slower)

				// Interpolate between MinOptimalDelta/MaxOptimalDelta based on how long it's been since this actor actually sent anything
				const float Alpha = Helpers::KismetMathLibrary()->Clamp((LastReplicateDelta - ScaleDownStartTime) / ScaleDownTimeRange, 0.0f, 1.0f);
				ActorInfo->OptimalNetUpdateDelta = Helpers::KismetMathLibrary()->Lerp(MinOptimalDelta, MaxOptimalDelta, Alpha);
			}

			ActorInfo->bPendingNetUpdate = false;
			OutConsiderList.Add(ActorInfo);
			Actor->CallPreReplication(NetDriver);
		}
	}

	//https://github.com/EpicGames/UnrealEngine/blob/37ca478f5aa37e9dd49b68a7a39d01a9d5937154/Engine/Source/Runtime/Engine/Private/NetworkDriver.cpp#L2818
	int32 ServerReplicateActors_PrioritizeActors(UNetDriver* NetDriver, UNetConnection* Connection, TArray<FNetViewer>& ConnectionViewers, TArray<FNetworkObjectInfo*> ConsiderList)
	{
		for (FNetworkObjectInfo* ActorInfo : ConsiderList)
		{
			AActor* Actor = ActorInfo->Actor;
			UActorChannel* Channel = FindChannel(Connection, Actor);
			UNetConnection* PriorityConnection = Connection;

			if (Actor->bOnlyRelevantToOwner)
			{
				bool bHasNullViewTarget = false;
				
				PriorityConnection = IsActorOwnedByAndRelevantToConnection(Actor, ConnectionViewers, bHasNullViewTarget);

				if (PriorityConnection)
					continue;

				if (!Channel)
				{
					if (!IsActorRelevantToConnection(Actor, ConnectionViewers))
						continue;
				}
			}
		}

		return 0;
	}


	//https://github.com/EpicGames/UnrealEngine/blob/37ca478f5aa37e9dd49b68a7a39d01a9d5937154/Engine/Source/Runtime/Engine/Private/NetworkDriver.cpp#L2949
	int32 ServerReplicateActors_ProcessPrioritizedActors(UNetDriver* NetDriver, UNetConnection* Connection, const TArray<FNetViewer>& ConnectionViewers, TArray<FNetworkObjectInfo*> ConsiderList)
	{
		for (FNetworkObjectInfo* ActorInfo : ConsiderList)
		{
			AActor* Actor = ActorInfo->Actor;
			UActorChannel* Channel = (UActorChannel*)Connection->CreateChannel(CHTYPE_Actor, true, -1);
			if (Channel)
			{
				Channel->SetChannelActor(Actor);
				Channel->ReplicateActor();
			}
		}

		return 0;
	}

	int32 ServerReplicateActors(UNetDriver* NetDriver, float DeltaSeconds)
	{
		if (NetDriver->ClientConnections.Num() == 0)
			return 0;

		NetDriver->GetReplicationFrame();

		int32 NumClientsToTick = ServerReplicateActors_PrepConnections(NetDriver, DeltaSeconds);

		if (NumClientsToTick == 0)
			return 0;

		TArray<FNetworkObjectInfo*> ConsiderList;
		ConsiderList.Reserve(NetDriver->GetNetworkObjectList().GetActiveObjects().Num());

		ServerReplicateActors_BuildConsiderList(NetDriver, ConsiderList);

		for (int32 i = 0; i < NetDriver->ClientConnections.Num(); i++)
		{
			UNetConnection* Connection = NetDriver->ClientConnections[i];
			if (!Connection)
				continue;

			if (i >= NumClientsToTick)
				continue;

			if (!Connection->ViewTarget)
				continue;

			TArray<FNetViewer>& ConnectionViewers = AWorldSettings::GetDefaultObj()->ReplicationViewers;

			GetFNetViewer(Connection);
			for (int32 ViewerIndex = 0; ViewerIndex < Connection->Children.Num(); ViewerIndex++)
			{
				if (Connection->Children[ViewerIndex]->ViewTarget)
					GetFNetViewer(Connection->Children[ViewerIndex]);
			}

			if (Connection->PlayerController)
				Connection->PlayerController->SendClientAdjustment();

			for (int32 ChildIdx = 0; ChildIdx < Connection->Children.Num(); ChildIdx++)
			{
				if (Connection->Children[ChildIdx]->PlayerController)
					Connection->Children[ChildIdx]->PlayerController->SendClientAdjustment();
			}

			ServerReplicateActors_PrioritizeActors(NetDriver, Connection, ConnectionViewers, ConsiderList);
			ServerReplicateActors_ProcessPrioritizedActors(NetDriver, Connection, ConnectionViewers, ConsiderList);
		}

		return 0;
	}

	void* (*_TickFlush)(UNetDriver* NetDriver, float DeltaSeconds);
	void* TickFlush(UNetDriver* NetDriver, float DeltaSeconds)
	{
		if (NetDriver->ServerConnection == nullptr && NetDriver->ClientConnections.Num() > 0 && !NetDriver->ClientConnections[0]->InternalAck)
			ServerReplicateActors(NetDriver, DeltaSeconds);
		return _TickFlush(NetDriver, DeltaSeconds);
	}

	static void InitHook()
	{
		Util::CreateHook(Util::GetOffset(0x22dcf00), TickFlush, &_TickFlush);
	}
}