#pragma once
#include "SDK/SDK.hpp"
using namespace SDK;

namespace SDK
{
	static FNetViewer GetFNetViewer(UNetConnection* InConnection)
	{
		FNetViewer NetViewer;
		NetViewer.Connection = InConnection;
		NetViewer.InViewer = InConnection->PlayerController ? InConnection->PlayerController : InConnection->OwningActor;
		NetViewer.ViewTarget = InConnection->ViewTarget;
		NetViewer.ViewLocation = InConnection->ViewTarget->K2_GetActorLocation();
		NetViewer.ViewDir = InConnection->ViewTarget->K2_GetActorLocation();
		return NetViewer;
	}

	enum EChannelType
	{
		CHTYPE_None = 0,  // Invalid type.
		CHTYPE_Control = 1,  // Connection control.
		CHTYPE_Actor = 2,  // Actor-update channel.
		CHTYPE_File = 3,  // Binary file transfer.
		CHTYPE_MAX = 32, // Maximum.
	};


	enum ENetMode
	{
		NM_Standalone,
		NM_DedicatedServer,
		NM_ListenServer,
		NM_Client,
		NM_MAX,
	};

	struct FNetworkObjectInfo
	{
		/** Pointer to the replicated actor. */
		AActor* Actor;

		/** List of connections that this actor is dormant on */
		TSet<TWeakObjectPtr<UNetConnection>> DormantConnections;

		/** A list of connections that this actor has recently been dormant on, but the actor doesn't have a channel open yet.
		*  These need to be differentiated from actors that the client doesn't know about, but there's no explicit list for just those actors.
		*  (this list will be very transient, with connections being moved off the DormantConnections list, onto this list, and then off once the actor has a channel again)
		*/
		TSet<TWeakObjectPtr<UNetConnection>> RecentlyDormantConnections;

		/** Next time to consider replicating the actor. Based on FPlatformTime::Seconds(). */
		double NextUpdateTime;

		/** Last absolute time in seconds since actor actually sent something during replication */
		double LastNetReplicateTime;

		/** Optimal delta between replication updates based on how frequently actor properties are actually changing */
		float OptimalNetUpdateDelta;

		/** Last time this actor was updated for replication via NextUpdateTime
		* @warning: internal net driver time, not related to WorldSettings.TimeSeconds */
		float LastNetUpdateTime;

		/** Is this object still pending a full net update due to clients that weren't able to replicate the actor at the time of LastNetUpdateTime */
		bool bPendingNetUpdate;

		FNetworkObjectInfo()
			: Actor(nullptr)
			, NextUpdateTime(0.0)
			, LastNetReplicateTime(0.0)
			, OptimalNetUpdateDelta(0.0f)
			, LastNetUpdateTime(0.0f)
			, bPendingNetUpdate(false) {}

		FNetworkObjectInfo(AActor* InActor)
			: Actor(InActor)
			, NextUpdateTime(0.0)
			, LastNetReplicateTime(0.0)
			, OptimalNetUpdateDelta(0.0f)
			, LastNetUpdateTime(0.0f)
			, bPendingNetUpdate(false) {}
	};

	class FNetworkObjectList
	{
	public:
		typedef TSet<TSharedPtr<FNetworkObjectInfo>> FNetworkObjectSet;

		/** Returns a const reference to the entire set of tracked actors. */
		const FNetworkObjectSet& GetAllObjects() const { return AllNetworkObjects; }

		/** Returns a const reference to the active set of tracked actors. */
		const FNetworkObjectSet& GetActiveObjects() const { return ActiveNetworkObjects; }

		/** Returns a const reference to the entire set of dormant actors. */
		const FNetworkObjectSet& GetDormantObjectsOnAllConnections() const { return ObjectsDormantOnAllConnections; }

	public:
		FNetworkObjectSet AllNetworkObjects;
		FNetworkObjectSet ActiveNetworkObjects;
		FNetworkObjectSet ObjectsDormantOnAllConnections;
		TMap<TWeakObjectPtr<UNetConnection>, int32 > NumDormantObjectsPerConnection;
	};
}