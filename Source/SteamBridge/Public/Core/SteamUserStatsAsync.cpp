// Copyright 2020-2024 Russ 'trdwll' Treadwell <trdwll.com>. All Rights Reserved.


#include "SteamUserStatsAsync.h"

#pragma region Attach Leaderboard UGC

USteamUserStats_AttachLeaderboardUGC* USteamUserStats_AttachLeaderboardUGC::AttachLeaderboardUGC(const FSteamLeaderboard SteamLeaderboard, const FUGCHandle UGC)
{
	USteamUserStats_AttachLeaderboardUGC* BlueprintNode = NewObject<USteamUserStats_AttachLeaderboardUGC>();
	BlueprintNode->SteamLeaderboard = SteamLeaderboard;
	BlueprintNode->UGC = UGC;
	return BlueprintNode;
}

void USteamUserStats_AttachLeaderboardUGC::Activate()
{
	Super::Activate();
	if (!SteamUserStats())
	{
		OnFailure.Broadcast(FSteamLeaderboard());
		SetReadyToDestroy();
		MarkAsGarbage();
		return;
	}

	CallbackHandle = SteamUserStats()->AttachLeaderboardUGC(SteamLeaderboard, UGC);
	if (CallbackHandle == k_uAPICallInvalid)
	{
		OnFailure.Broadcast(FSteamLeaderboard());
		SetReadyToDestroy();
		MarkAsGarbage();
		return;
	}

	OnAttachLeaderboardUGCCallResult.Set(CallbackHandle, this, &USteamUserStats_AttachLeaderboardUGC::OnAttachLeaderboardUGC);
}

void USteamUserStats_AttachLeaderboardUGC::OnAttachLeaderboardUGC(LeaderboardUGCSet_t* pResult, bool bIOFailure)
{
	AsyncTask(ENamedThreads::GameThread, [this, Param = *pResult, bIOFailure]()
	{
		if (bIOFailure)
		{
			OnFailure.Broadcast(-1);
			SetReadyToDestroy();
			MarkAsGarbage();
			return;
		}

		OnSuccess.Broadcast(Param.m_hSteamLeaderboard);
	});

	SetReadyToDestroy();
	MarkAsGarbage();
}

#pragma endregion Attach Leaderboard UGC

#pragma region Download Leaderboard Entries

USteamUserStats_DownloadLeaderboardEntries* USteamUserStats_DownloadLeaderboardEntries::DownloadLeaderboardEntries(const FSteamLeaderboard SteamLeaderboard, const ESteamLeaderboardDataRequest LeaderboardDataRequest, const int32 RangeStart, const int32 RangeEnd)
{
	USteamUserStats_DownloadLeaderboardEntries* BlueprintNode = NewObject<USteamUserStats_DownloadLeaderboardEntries>();
	BlueprintNode->SteamLeaderboard = SteamLeaderboard;
	BlueprintNode->LeaderboardDataRequest = LeaderboardDataRequest;
	BlueprintNode->RangeStart = RangeStart;
	BlueprintNode->RangeEnd = RangeEnd;
	return BlueprintNode;
}

void USteamUserStats_DownloadLeaderboardEntries::Activate()
{
	Super::Activate();
	if (!SteamUserStats())
	{
		OnFailure.Broadcast(FSteamLeaderboardScoresDownloaded(-1, -1));
		SetReadyToDestroy();
		MarkAsGarbage();
		return;
	}

	CallbackHandle = SteamUserStats()->DownloadLeaderboardEntries(SteamLeaderboard, static_cast<ELeaderboardDataRequest>(LeaderboardDataRequest), RangeStart, RangeEnd);
	if (CallbackHandle == k_uAPICallInvalid)
	{
		OnFailure.Broadcast(FSteamLeaderboardScoresDownloaded(-1, -1));
		SetReadyToDestroy();
		MarkAsGarbage();
		return;
	}

	OnDownloadLeaderboardEntriesCallResult.Set(CallbackHandle, this, &USteamUserStats_DownloadLeaderboardEntries::OnDownloadLeaderboardEntries);
}

void USteamUserStats_DownloadLeaderboardEntries::OnDownloadLeaderboardEntries(LeaderboardScoresDownloaded_t* pResult, bool bIOFailure)
{
	AsyncTask(ENamedThreads::GameThread, [this, Param = *pResult, bIOFailure]()
	{
		if (bIOFailure)
		{
			OnFailure.Broadcast(FSteamLeaderboardScoresDownloaded(-1, -1));
			SetReadyToDestroy();
			MarkAsGarbage();
			return;
		}

		OnSuccess.Broadcast(FSteamLeaderboardScoresDownloaded(Param.m_hSteamLeaderboard, Param.m_hSteamLeaderboardEntries));
	});

	SetReadyToDestroy();
	MarkAsGarbage();
}


#pragma endregion Download Leaderboard Entries

#pragma region Download Leaderboard Entries For Users

USteamUserStats_DownloadLeaderboardEntriesForUsers* USteamUserStats_DownloadLeaderboardEntriesForUsers::DownloadLeaderboardEntriesForUsers(const FSteamLeaderboard SteamLeaderboard, const TArray<FSteamID>& SteamIDs)
{
	USteamUserStats_DownloadLeaderboardEntriesForUsers* BlueprintNode = NewObject<USteamUserStats_DownloadLeaderboardEntriesForUsers>();
	BlueprintNode->SteamLeaderboard = SteamLeaderboard;
	BlueprintNode->SteamIDs = SteamIDs;
	BlueprintNode->Count = SteamIDs.Num();
	return BlueprintNode;
}

void USteamUserStats_DownloadLeaderboardEntriesForUsers::Activate()
{
	Super::Activate();
	if (!SteamUserStats())
	{
		OnFailure.Broadcast(FSteamLeaderboardScoresDownloaded(-1, -1));
		SetReadyToDestroy();
		MarkAsGarbage();
		return;
	}

	TArray<CSteamID> CSteamIDs;
	CSteamIDs.Reserve(SteamIDs.Num());
	for (const FSteamID& SteamID : SteamIDs)
	{
		CSteamIDs.Add(CSteamID(SteamID.Value));
	}

	CallbackHandle = SteamUserStats()->DownloadLeaderboardEntriesForUsers(SteamLeaderboard, CSteamIDs.GetData(), CSteamIDs.Num());
	if (CallbackHandle == k_uAPICallInvalid)
	{
		OnFailure.Broadcast(FSteamLeaderboardScoresDownloaded(-1, -1));
		SetReadyToDestroy();
		MarkAsGarbage();
		return;
	}

	OnDownloadLeaderboardEntriesCallResult.Set(CallbackHandle, this, &USteamUserStats_DownloadLeaderboardEntriesForUsers::OnDownloadLeaderboardEntriesForUsers);
}

void USteamUserStats_DownloadLeaderboardEntriesForUsers::OnDownloadLeaderboardEntriesForUsers(LeaderboardScoresDownloaded_t* pResult, bool bIOFailure)
{
	AsyncTask(ENamedThreads::GameThread, [this, Param = *pResult, bIOFailure]()
	{
		if (bIOFailure)
		{
			OnFailure.Broadcast(FSteamLeaderboardScoresDownloaded(-1, -1));
			SetReadyToDestroy();
			MarkAsGarbage();
			return;
		}

		OnSuccess.Broadcast(FSteamLeaderboardScoresDownloaded(Param.m_hSteamLeaderboard, Param.m_hSteamLeaderboardEntries)); });

	SetReadyToDestroy();
	MarkAsGarbage();
}

#pragma endregion Download Leaderboard Entries For Users

#pragma region Find or Create Leaderboard

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

#pragma endregion Find or Create Leaderboard

#pragma region Find Leaderboard

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

#pragma endregion Find Leaderboard

#pragma region Get Number of Current Players

USteamUserStats_GetNumberOfCurrentPlayers* USteamUserStats_GetNumberOfCurrentPlayers::GetNumberOfCurrentPlayers()
{
	USteamUserStats_GetNumberOfCurrentPlayers* BlueprintNode = NewObject<USteamUserStats_GetNumberOfCurrentPlayers>();
	return BlueprintNode;
}

void USteamUserStats_GetNumberOfCurrentPlayers::Activate()
{
	Super::Activate();
	if (!SteamUserStats())
	{
		OnFailure.Broadcast(-1);
		SetReadyToDestroy();
		MarkAsGarbage();
		return;
	}

	CallbackHandle = SteamUserStats()->GetNumberOfCurrentPlayers();
	if (CallbackHandle == k_uAPICallInvalid)
	{
		OnFailure.Broadcast(-1);
		SetReadyToDestroy();
		MarkAsGarbage();
		return;
	}

	OnGetNumberOfCurrentPlayersCallResult.Set(CallbackHandle, this, &USteamUserStats_GetNumberOfCurrentPlayers::OnGetNumberOfCurrentPlayers);
}

void USteamUserStats_GetNumberOfCurrentPlayers::OnGetNumberOfCurrentPlayers(NumberOfCurrentPlayers_t* pResult, bool bIOFailure)
{
	AsyncTask(ENamedThreads::GameThread, [this, Param = *pResult, bIOFailure]()
	{
		if (bIOFailure)
		{
			OnFailure.Broadcast(-1);
			SetReadyToDestroy();
			MarkAsGarbage();
			return;
		}

		OnSuccess.Broadcast(Param.m_cPlayers);
	});

	SetReadyToDestroy();
	MarkAsGarbage();
}

#pragma endregion Get Number of Current Players
