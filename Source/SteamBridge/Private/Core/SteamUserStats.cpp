// Copyright 2020-2022 Russ 'trdwll' Treadwell <trdwll.com>. All Rights Reserved.

#include "Core/SteamUserStats.h"

#include "SteamBridgeUtils.h"

USteamUserStats::USteamUserStats()
{
	OnGlobalAchievementPercentagesReadyCallback.Register(this, &USteamUserStats::OnGlobalAchievementPercentagesReady);
	OnGlobalStatsReceivedCallback.Register(this, &USteamUserStats::OnGlobalStatsReceived);
	OnLeaderboardFindResultCallback.Register(this, &USteamUserStats::OnLeaderboardFindResult);
	OnLeaderboardScoresDownloadedCallback.Register(this, &USteamUserStats::OnLeaderboardScoresDownloaded);
	OnLeaderboardScoreUploadedCallback.Register(this, &USteamUserStats::OnLeaderboardScoreUploaded);
	OnLeaderboardUGCSetCallback.Register(this, &USteamUserStats::OnLeaderboardUGCSet);
	OnNumberOfCurrentPlayersCallback.Register(this, &USteamUserStats::OnNumberOfCurrentPlayers);
	OnUserAchievementIconFetchedCallback.Register(this, &USteamUserStats::OnUserAchievementIconFetched);
	OnUserAchievementStoredCallback.Register(this, &USteamUserStats::OnUserAchievementStored);
	OnUserStatsReceivedCallback.Register(this, &USteamUserStats::OnUserStatsReceived);
	OnUserStatsStoredCallback.Register(this, &USteamUserStats::OnUserStatsStored);
	OnUserStatsUnloadedCallback.Register(this, &USteamUserStats::OnUserStatsUnloaded);

	if (IsRunningDedicatedServer())
	{
		OnGlobalAchievementPercentagesReadyCallback.SetGameserverFlag();
		OnGlobalStatsReceivedCallback.SetGameserverFlag();
		OnLeaderboardFindResultCallback.SetGameserverFlag();
		OnLeaderboardScoresDownloadedCallback.SetGameserverFlag();
		OnLeaderboardScoreUploadedCallback.SetGameserverFlag();
		OnLeaderboardUGCSetCallback.SetGameserverFlag();
		OnNumberOfCurrentPlayersCallback.SetGameserverFlag();
		OnUserAchievementIconFetchedCallback.SetGameserverFlag();
		OnUserAchievementStoredCallback.SetGameserverFlag();
		OnUserStatsReceivedCallback.SetGameserverFlag();
		OnUserStatsStoredCallback.SetGameserverFlag();
		OnUserStatsUnloadedCallback.SetGameserverFlag();
	}
}

USteamUserStats::~USteamUserStats()
{
	OnGlobalAchievementPercentagesReadyCallback.Unregister();
	OnGlobalStatsReceivedCallback.Unregister();
	OnLeaderboardFindResultCallback.Unregister();
	OnLeaderboardScoresDownloadedCallback.Unregister();
	OnLeaderboardScoreUploadedCallback.Unregister();
	OnLeaderboardUGCSetCallback.Unregister();
	OnNumberOfCurrentPlayersCallback.Unregister();
	OnUserAchievementIconFetchedCallback.Unregister();
	OnUserAchievementStoredCallback.Unregister();
	OnUserStatsReceivedCallback.Unregister();
	OnUserStatsStoredCallback.Unregister();
	OnUserStatsUnloadedCallback.Unregister();
}

bool USteamUserStats::GetAchievementAndUnlockTime(const FString& Name, bool& bAchieved, FDateTime& UnlockTime) const
{
	uint32 TmpTime;
	bool bResult = SteamUserStats()->GetAchievementAndUnlockTime(TCHAR_TO_UTF8(*Name), &bAchieved, &TmpTime);
	UnlockTime = FDateTime::FromUnixTimestamp(TmpTime);
	return bResult;
}

bool USteamUserStats::GetDownloadedLeaderboardEntry(const FSteamLeaderboardEntries SteamLeaderboardEntries, const int32 index, FSteamLeaderboardEntry& LeaderboardEntry, TArray<int32>& Details, const int32 DetailsMax) const
{
	Details.SetNum(DetailsMax);
	LeaderboardEntry_t TmpEntry;
	bool bResult = SteamUserStats()->GetDownloadedLeaderboardEntry(SteamLeaderboardEntries, index, &TmpEntry, Details.GetData(), DetailsMax);
	LeaderboardEntry = TmpEntry;
	return bResult;
}

int32 USteamUserStats::GetGlobalStatHistoryFloat(const FString& StatName, TArray<float>& Data, const int32 Size /*= 10*/) const
{
	TArray<double> TmpData;
	TmpData.Reserve(Size);
	int32 result = SteamUserStats()->GetGlobalStatHistory(TCHAR_TO_UTF8(*StatName), TmpData.GetData(), Size);
	for (const auto& LData : TmpData)
	{
		Data.Add((float)LData);
	}
	return result;
}

int32 USteamUserStats::GetMostAchievedAchievementInfo(FString& Name, float& Percent, bool& bAchieved) const
{
	TArray<char> TmpName;
	TmpName.Reserve(SteamDefs::Buffer1024);
	int32 result = SteamUserStats()->GetMostAchievedAchievementInfo(TmpName.GetData(), SteamDefs::Buffer1024, &Percent, &bAchieved);
	Name = UTF8_TO_TCHAR(TmpName.GetData());
	return  result;
}

int32 USteamUserStats::GetNextMostAchievedAchievementInfo(const int32 IteratorPrevious, FString& Name, float& Percent, bool& bAchieved) const
{
	TArray<char> TmpName;
	TmpName.Reserve(SteamDefs::Buffer1024);
	int32 result = SteamUserStats()->GetNextMostAchievedAchievementInfo(IteratorPrevious, TmpName.GetData(), SteamDefs::Buffer1024, &Percent, &bAchieved);
	Name = UTF8_TO_TCHAR(TmpName.GetData());
	return  result;
}

bool USteamUserStats::GetUserAchievementAndUnlockTime(FSteamID SteamIDUser, const FString& Name, bool& bAchieved, FDateTime& UnlockTime) const
{
	uint32 TmpTime;
	bool bResult = SteamUserStats()->GetUserAchievementAndUnlockTime(SteamIDUser, TCHAR_TO_UTF8(*Name), &bAchieved, &TmpTime);
	UnlockTime = FDateTime::FromUnixTimestamp(TmpTime);
	return bResult;
}

FSteamAPICall USteamUserStats::UploadLeaderboardScore(const FSteamLeaderboard SteamLeaderboard, const ESteamLeaderboardUploadScoreMethod LeaderboardUploadScoreMethod, const int32 Score, const TArray<int32>& ScoreDetails) const
{
	return SteamUserStats()->UploadLeaderboardScore(SteamLeaderboard, (ELeaderboardUploadScoreMethod)LeaderboardUploadScoreMethod, Score, ScoreDetails.GetData(), ScoreDetails.Num());
}

void USteamUserStats::OnGlobalAchievementPercentagesReady(GlobalAchievementPercentagesReady_t* pParam)
{
	AsyncTask(ENamedThreads::GameThread, [this, Param = *pParam]()
	{
		OnGlobalAchievementPercentagesReadyDelegate.Broadcast(Param.m_nGameID, static_cast<ESteamResult>(Param.m_eResult));
	});
}

void USteamUserStats::OnGlobalStatsReceived(GlobalStatsReceived_t* pParam)
{
	AsyncTask(ENamedThreads::GameThread, [this, Param = *pParam]()
	{
		OnGlobalStatsReceivedDelegate.Broadcast(Param.m_nGameID, static_cast<ESteamResult>(Param.m_eResult));
	});
}

void USteamUserStats::OnLeaderboardFindResult(LeaderboardFindResult_t* pParam)
{
	AsyncTask(ENamedThreads::GameThread, [this, Param = *pParam]()
	{
		OnLeaderboardFindResultDelegate.Broadcast(Param.m_hSteamLeaderboard, Param.m_bLeaderboardFound == 1);
	});
}

void USteamUserStats::OnLeaderboardScoresDownloaded(LeaderboardScoresDownloaded_t* pParam)
{
	AsyncTask(ENamedThreads::GameThread, [this, Param = *pParam]()
	{
		OnLeaderboardScoresDownloadedDelegate.Broadcast(Param.m_hSteamLeaderboard, Param.m_hSteamLeaderboardEntries, Param.m_cEntryCount);
	});
}

void USteamUserStats::OnLeaderboardScoreUploaded(LeaderboardScoreUploaded_t* pParam) 
{
	AsyncTask(ENamedThreads::GameThread, [this, Param = *pParam]()
	{
		OnLeaderboardScoreUploadedDelegate.Broadcast(Param.m_bSuccess == 1, Param.m_hSteamLeaderboard, Param.m_nScore, Param.m_bScoreChanged == 1, Param.m_nGlobalRankNew, Param.m_nGlobalRankPrevious);
	});
}

void USteamUserStats::OnLeaderboardUGCSet(LeaderboardUGCSet_t* pParam)
{
	AsyncTask(ENamedThreads::GameThread, [this, Param = *pParam]()
	{
		OnLeaderboardUGCSetDelegate.Broadcast(static_cast<ESteamResult>(Param.m_eResult), Param.m_hSteamLeaderboard);
	});
}

void USteamUserStats::OnNumberOfCurrentPlayers(NumberOfCurrentPlayers_t* pParam)
{
	AsyncTask(ENamedThreads::GameThread, [this, Param = *pParam]()
	{
		OnNumberOfCurrentPlayersDelegate.Broadcast(Param.m_bSuccess == 1, Param.m_cPlayers);
	});
}

void USteamUserStats::OnUserAchievementIconFetched(UserAchievementIconFetched_t* pParam)
{
	AsyncTask(ENamedThreads::GameThread, [this, Param = *pParam]()
	{
		OnUserAchievementIconFetchedDelegate.Broadcast(Param.m_nGameID.ToUint64(), UTF8_TO_TCHAR(Param.m_rgchAchievementName), Param.m_bAchieved, Param.m_nIconHandle);
	});
}

void USteamUserStats::OnUserAchievementStored(UserAchievementStored_t* pParam)
{
	AsyncTask(ENamedThreads::GameThread, [this, Param = *pParam]()
	{
		OnUserAchievementStoredDelegate.Broadcast(Param.m_nGameID, Param.m_bGroupAchievement, UTF8_TO_TCHAR(Param.m_rgchAchievementName), Param.m_nCurProgress, Param.m_nMaxProgress);
	});
}

void USteamUserStats::OnUserStatsReceived(UserStatsReceived_t* pParam)
{
	AsyncTask(ENamedThreads::GameThread, [this, Param = *pParam]()
	{
		OnUserStatsReceivedDelegate.Broadcast(Param.m_nGameID, static_cast<ESteamResult>(Param.m_eResult), Param.m_steamIDUser.ConvertToUint64());
	});
}

void USteamUserStats::OnUserStatsStored(UserStatsStored_t* pParam)
{
	AsyncTask(ENamedThreads::GameThread, [this, Param = *pParam]()
	{
		OnUserStatsStoredDelegate.Broadcast(Param.m_nGameID, static_cast<ESteamResult>(Param.m_eResult));
	});
}

void USteamUserStats::OnUserStatsUnloaded(UserStatsUnloaded_t* pParam)
{
	AsyncTask(ENamedThreads::GameThread, [this, Param = *pParam]()
	{
		OnUserStatsUnloadedDelegate.Broadcast(Param.m_steamIDUser.ConvertToUint64());
	});
}
