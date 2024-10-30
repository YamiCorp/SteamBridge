// Copyright 2020-2024 Russ 'trdwll' Treadwell <trdwll.com>. All Rights Reserved.

#pragma once
#include <CoreMinimal.h>

#include <Kismet/BlueprintAsyncActionBase.h>

#include "SteamStructs.h"

#include "SteamUserStatsAsync.generated.h"

USTRUCT(BlueprintType)
struct FSteamLeaderboardScoresDownloaded
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FSteamLeaderboard Leaderboard;

	UPROPERTY(BlueprintReadWrite)
	FSteamLeaderboardEntries LeaderboardEntries;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttachLeaderboardUGCDelegate, const FSteamLeaderboard&, Leaderboard);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFindLeaderboardDelegate, const FSteamLeaderboard&, Leaderboard);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFindOrCreateLeaderboardDelegate, const FSteamLeaderboard&, Leaderboard);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDownloadLeaderboardEntriesDelegate, const FSteamLeaderboardScoresDownloaded&, LeaderboardEntry);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDownloadLeaderboardEntriesForUsersDelegate, const FSteamLeaderboardScoresDownloaded&, LeaderboardEntry);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNumberOfCurrentPlayersReceived, int32, NumberOfCurrentPlayers);

UCLASS()
class STEAMBRIDGE_API USteamUserStats_AttachLeaderboardUGC : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	/**
	 * Attaches a piece of user generated content the current user's entry on a leaderboard.
	 * This content could be a replay of the user achieving the score or a ghost to race against. The attached handle will be available when the entry is retrieved and can be accessed by other users using
	 * `1	`GetDownloadedLeaderboardEntry which contains LeaderboardEntry_t.m_hUGC. To create and download user generated content see the documentation for the Steam Workshop.
	 * Once attached, the content will be available even if the underlying Cloud file is changed or deleted by the user.
	 * You must call FindLeaderboard or FindOrCreateLeaderboard to get a SteamLeaderboard_t prior to calling this function.
	 *
	 * @param FSteamLeaderboard SteamLeaderboard - A leaderboard handle obtained from FindLeaderboard or FindOrCreateLeaderboard.
	 * @param FUGCHandle UGC - Handle to a piece of user generated content that was shared using ISteamRemoteStorage::FileShare or ISteamUGC::CreateItem.
	 * @return FSteamAPICall - SteamAPICall_t to be used with a LeaderboardUGCSet_t call result.
	 */
	UFUNCTION(BlueprintCallable, DisplayName = "Attach Leaderboard UGC", meta = (BlueprintInternalUseOnly = "true"), Category = "SteamBridgeCore|UserStats")
	static USteamUserStats_AttachLeaderboardUGC* AttachLeaderboardUGC(const FSteamLeaderboard SteamLeaderboard, const FUGCHandle UGC);

	UPROPERTY(BlueprintAssignable)
	FAttachLeaderboardUGCDelegate OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FAttachLeaderboardUGCDelegate OnFailure;

private:
	SteamAPICall_t CallbackHandle;
	FSteamLeaderboard SteamLeaderboard;
	FUGCHandle UGC;

	virtual void Activate() override;

	void OnAttachLeaderboardUGC(LeaderboardUGCSet_t* pResult, bool bIOFailure);
	CCallResult<USteamUserStats_AttachLeaderboardUGC, LeaderboardUGCSet_t> OnAttachLeaderboardUGCCallResult;
};

UCLASS()
class STEAMBRIDGE_API USteamUserStats_DownloadLeaderboardEntries : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	/**
	 * Fetches a series of leaderboard entries for a specified leaderboard.
	 * You can ask for more entries than exist, then this will return as many as do exist.
	 * If you want to download entries for an arbitrary set of users, such as all of the users on a server then you can use DownloadLeaderboardEntriesForUsers which takes an array of Steam IDs.
	 * You must call FindLeaderboard or FindOrCreateLeaderboard to get a SteamLeaderboard_t prior to calling this function.
	 *
	 * @param FSteamLeaderboard SteamLeaderboard - A leaderboard handle obtained from FindLeaderboard or FindOrCreateLeaderboard.
	 * @param ESteamLeaderboardDataRequest LeaderboardDataRequest - The type of data request to make.
	 * @param int32 RangeStart - The index to start downloading entries relative to eLeaderboardDataRequest.
	 * @param int32 RangeEnd - The last index to retrieve entries for relative to eLeaderboardDataRequest.
	 * @return FSteamAPICall - SteamAPICall_t to be used with a LeaderboardScoresDownloaded_t call result.
	 */
	UFUNCTION(BlueprintCallable, DisplayName = "Download Leaderboard Entries", meta = (BlueprintInternalUseOnly = "true"), Category = "SteamBridgeCore|UserStats")
	static USteamUserStats_DownloadLeaderboardEntries* DownloadLeaderboardEntries(const FSteamLeaderboard SteamLeaderboard, const ESteamLeaderboardDataRequest LeaderboardDataRequest, const int32 RangeStart, const int32 RangeEnd);

	UPROPERTY(BlueprintAssignable)
	FDownloadLeaderboardEntriesDelegate OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FDownloadLeaderboardEntriesDelegate OnFailure;

private:
	SteamAPICall_t CallbackHandle;
	FSteamLeaderboard SteamLeaderboard;
	ESteamLeaderboardDataRequest LeaderboardDataRequest = ESteamLeaderboardDataRequest::Global;
	int32 RangeStart = 0;
	int32 RangeEnd = 0;

	virtual void Activate() override;

	void OnDownloadLeaderboardEntries(LeaderboardScoresDownloaded_t* pResult, bool bIOFailure);
	CCallResult<USteamUserStats_DownloadLeaderboardEntries, LeaderboardScoresDownloaded_t> OnDownloadLeaderboardEntriesCallResult;
};

UCLASS()
class STEAMBRIDGE_API USteamUserStats_DownloadLeaderboardEntriesForUsers : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	/**
	 * Fetches leaderboard entries for an arbitrary set of users on a specified leaderboard.
	 * A maximum of 100 users can be downloaded at a time, with only one outstanding call at a time. If a user doesn't have an entry on the specified leaderboard, they won't be included in the result.
	 * If you want to download entries based on their ranking or friends of the current user then you should use DownloadLeaderboardEntries.
	 * You must call FindLeaderboard or FindOrCreateLeaderboard to get a SteamLeaderboard_t prior to calling this function.
	 *
	 * @param FSteamLeaderboard SteamLeaderboard - A leaderboard handle obtained from FindLeaderboard or FindOrCreateLeaderboard.
	 * @param const TArray<FSteamID>& SteamIds - An array of Steam IDs to get the leaderboard entries for.
	 * @return FSteamAPICall - SteamAPICall_t to be used with a LeaderboardScoresDownloaded_t call result.
	 */
	UFUNCTION(BlueprintCallable, DisplayName = "Download Leaderboard Entries For Users", meta = (BlueprintInternalUseOnly = "true"), Category = "SteamBridgeCore|UserStats")
	static USteamUserStats_DownloadLeaderboardEntriesForUsers* DownloadLeaderboardEntriesForUsers(const FSteamLeaderboard SteamLeaderboard, const TArray<FSteamID>& SteamIDs);

	UPROPERTY(BlueprintAssignable)
	FDownloadLeaderboardEntriesForUsersDelegate OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FDownloadLeaderboardEntriesForUsersDelegate OnFailure;

private:
	SteamAPICall_t CallbackHandle;
	FSteamLeaderboard SteamLeaderboard;
	TArray<FSteamID> SteamIDs;
	int32 Count = 0;

	virtual void Activate() override;

	void OnDownloadLeaderboardEntriesForUsers(LeaderboardScoresDownloaded_t* pResult, bool bIOFailure);
	CCallResult<USteamUserStats_DownloadLeaderboardEntriesForUsers, LeaderboardScoresDownloaded_t> OnDownloadLeaderboardEntriesCallResult;
};

UCLASS()
class STEAMBRIDGE_API USteamUserStats_FindOrCreateLeaderboard : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	/**
	 * Gets a leaderboard by name, it will create it if it's not yet created.
	 * You must call either this or FindLeaderboard to obtain the leaderboard handle which is valid for the game session for each leaderboard you wish to access prior to calling any other Leaderboard functions.
	 * Leaderboards created with this function will not automatically show up in the Steam Community. You must manually set the Community Name field in the App Admin panel of the Steamworks website. As such it's generally
	 * recommended to prefer creating the leaderboards in the App Admin panel on the Steamworks website and using FindLeaderboard unless you're expected to have a large amount of dynamically created leaderboards.
	 * You should never pass k_ELeaderboardSortMethodNone for eLeaderboardSortMethod or k_ELeaderboardDisplayTypeNone for eLeaderboardDisplayType as this is undefined behavior.
	 *
	 * @param const FString & LeaderboardName - The name of the leaderboard to find or create. Must not be longer than k_cchLeaderboardNameMax.
	 * @param ESteamLeaderboardSortMethod LeaderboardSortMethod - The sort order of the new leaderboard if it's created.
	 * @param ESteamLeaderboardDisplayType LeaderboardDisplayType - The display type (used by the Steam Community web site) of the new leaderboard if it's created.
	 * @return FSteamAPICall - SteamAPICall_t to be used with a LeaderboardFindResult_t call result.
	 */
	UFUNCTION(BlueprintCallable, DisplayName = "Find or Create Steam Leaderboard", meta = (BlueprintInternalUseOnly = "true"), Category = "SteamBridgeCore|UserStats")
	static USteamUserStats_FindOrCreateLeaderboard* FindOrCreateLeaderboard(const FString& LeaderboardName, const ESteamLeaderboardSortMethod LeaderboardSortMethod, const ESteamLeaderboardDisplayType LeaderboardDisplayType);

	UPROPERTY(BlueprintAssignable)
	FFindOrCreateLeaderboardDelegate OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FFindOrCreateLeaderboardDelegate OnFailure;

private:
	SteamAPICall_t CallbackHandle;
	FString LeaderboardName;
	ESteamLeaderboardSortMethod LeaderboardSortMethod = ESteamLeaderboardSortMethod::None;
	ESteamLeaderboardDisplayType LeaderboardDisplayType = ESteamLeaderboardDisplayType::None;

	virtual void Activate() override;

	void OnFindOrCreateLeaderboard(LeaderboardFindResult_t* pResult, bool bIOFailure);
	CCallResult<USteamUserStats_FindOrCreateLeaderboard, LeaderboardFindResult_t> OnFindLeaderboardCallResult;
};

UCLASS()
class STEAMBRIDGE_API USteamUserStats_FindLeaderboard : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	/**
	 * Gets a leaderboard by name.
	 * You must call either this or FindOrCreateLeaderboard to obtain the leaderboard handle which is valid for the game session for each leaderboard you wish to access prior to calling any other Leaderboard functions.
	 *
	 * @param const FString & LeaderboardName - The name of the leaderboard to find. Must not be longer than k_cchLeaderboardNameMax.
	 * @return FSteamAPICall - SteamAPICall_t to be used with a LeaderboardFindResult_t call result.
	 */
	UFUNCTION(BlueprintCallable, DisplayName = "Find Steam Leaderboard", meta = (BlueprintInternalUseOnly = "true"), Category = "SteamBridgeCore|UserStats")
	static USteamUserStats_FindLeaderboard* FindLeaderboard(const FString& LeaderboardName);

	UPROPERTY(BlueprintAssignable)
	FFindLeaderboardDelegate OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FFindLeaderboardDelegate OnFailure;

private:
	SteamAPICall_t CallbackHandle;
	FString LeaderboardName;

	virtual void Activate() override;

	void OnFindLeaderboard(LeaderboardFindResult_t* pResult, bool bIOFailure);
	CCallResult<USteamUserStats_FindLeaderboard, LeaderboardFindResult_t> OnFindLeaderboardCallResult;
};

UCLASS()
class STEAMBRIDGE_API USteamUserStats_GetNumberOfCurrentPlayers : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	/**
	 * Asynchronously retrieves the total number of players currently playing the current game. Both online and in offline mode.
	 *
	 * @return FSteamAPICall - SteamAPICall_t to be used with a NumberOfCurrentPlayers_t call result.
	 */
	UFUNCTION(BlueprintCallable, DisplayName = "Get Number of Current Players", meta = (BlueprintInternalUseOnly = "true"), Category = "SteamBridgeCore|UserStats")
	static USteamUserStats_GetNumberOfCurrentPlayers* GetNumberOfCurrentPlayers();

	UPROPERTY(BlueprintAssignable)
	FNumberOfCurrentPlayersReceived OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FNumberOfCurrentPlayersReceived OnFailure;

private:
	SteamAPICall_t CallbackHandle;

	virtual void Activate() override;

	void OnGetNumberOfCurrentPlayers(NumberOfCurrentPlayers_t* pResult, bool bIOFailure);
	CCallResult<USteamUserStats_GetNumberOfCurrentPlayers, NumberOfCurrentPlayers_t> OnGetNumberOfCurrentPlayersCallResult;
};

/*
 * TODO:
 * RequestGlobalAchievementPercentages
 * RequestGlobalStats
 * UploadLeaderboardScore
 * DownloadLeaderboardEntries
 * DownloadLeaderboardEntriesForUsers
 * RequestCurrentStats?
 */