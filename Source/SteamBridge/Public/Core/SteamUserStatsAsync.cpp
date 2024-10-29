// Copyright 2020-2024 Russ 'trdwll' Treadwell <trdwll.com>. All Rights Reserved.


#include "SteamUserStatsAsync.h"

// Start Find or Create Leaderboard

USteamUserStats_FindOrCreateLeaderboard* USteamUserStats_FindOrCreateLeaderboard::FindOrCreateLeaderboard(const FString& LeaderboardName, const ESteamLeaderboardSortMethod LeaderboardSortMethod, const ESteamLeaderboardDisplayType LeaderboardDisplayType)
{
	USteamUserStats_FindOrCreateLeaderboard* BlueprintNode = NewObject<USteamUserStats_FindOrCreateLeaderboard>();
	BlueprintNode->LeaderboardName = LeaderboardName;
	BlueprintNode->LeaderboardSortMethod = LeaderboardSortMethod;
	BlueprintNode->LeaderboardDisplayType = LeaderboardDisplayType;
	return BlueprintNode;
}

void USteamUserStats_FindOrCreateLeaderboard::Activate()
{
	Super::Activate();
	if (!SteamUserStats())
	{
		OnFailure.Broadcast(INDEX_NONE);
		SetReadyToDestroy();
		MarkAsGarbage();
		return;
	}

	CallbackHandle = SteamUserStats()->FindOrCreateLeaderboard(TCHAR_TO_UTF8(*LeaderboardName), static_cast<ELeaderboardSortMethod>(LeaderboardSortMethod), static_cast<ELeaderboardDisplayType>(LeaderboardDisplayType));
	if (CallbackHandle == k_uAPICallInvalid)
	{
		OnFailure.Broadcast(INDEX_NONE);
		SetReadyToDestroy();
		MarkAsGarbage();
		return;
	}

	OnFindLeaderboardCallResult.Set(CallbackHandle, this, &USteamUserStats_FindOrCreateLeaderboard::OnFindOrCreateLeaderboard);
}

void USteamUserStats_FindOrCreateLeaderboard::OnFindOrCreateLeaderboard(LeaderboardFindResult_t* pResult, bool bIOFailure)
{
	AsyncTask(ENamedThreads::GameThread, [this, Param = *pResult, bIOFailure]()
	{
		if (bIOFailure)
		{
			OnFailure.Broadcast(INDEX_NONE);
			SetReadyToDestroy();
			MarkAsGarbage();
			return;
		}

		if(Param.m_bLeaderboardFound)
		{
			OnSuccess.Broadcast(Param.m_hSteamLeaderboard);
		}
		else
		{
			OnFailure.Broadcast(INDEX_NONE);
		}
	});

	SetReadyToDestroy();
	MarkAsGarbage();
}

// End Find or Create Leaderboard

// Start Find Leaderboard

USteamUserStats_FindLeaderboard* USteamUserStats_FindLeaderboard::FindLeaderboard(const FString& LeaderboardName)
{
	USteamUserStats_FindLeaderboard* BlueprintNode = NewObject<USteamUserStats_FindLeaderboard>();
	BlueprintNode->LeaderboardName = LeaderboardName;
	return BlueprintNode;
}

void USteamUserStats_FindLeaderboard::Activate()
{
	Super::Activate();
	if (!SteamUserStats())
	{
		OnFailure.Broadcast(INDEX_NONE);
		SetReadyToDestroy();
		MarkAsGarbage();
		return;
	}

	CallbackHandle = SteamUserStats()->FindLeaderboard(TCHAR_TO_UTF8(*LeaderboardName));
	if (CallbackHandle == k_uAPICallInvalid)
	{
		OnFailure.Broadcast(INDEX_NONE);
		SetReadyToDestroy();
		MarkAsGarbage();
		return;
	}

	OnFindLeaderboardCallResult.Set(CallbackHandle, this, &USteamUserStats_FindLeaderboard::OnFindLeaderboard);
}

void USteamUserStats_FindLeaderboard::OnFindLeaderboard(LeaderboardFindResult_t* pResult, bool bIOFailure)
{
	AsyncTask(ENamedThreads::GameThread, [this, Param = *pResult, bIOFailure]()
	{
		if (bIOFailure)
		{
			OnFailure.Broadcast(INDEX_NONE);
			SetReadyToDestroy();
			MarkAsGarbage();
			return;
		}

		if(Param.m_bLeaderboardFound)
		{
			OnSuccess.Broadcast(Param.m_hSteamLeaderboard);
		}
		else
		{
			OnFailure.Broadcast(INDEX_NONE);
		}
	});

	SetReadyToDestroy();
	MarkAsGarbage();
}

// End Find Leaderboard