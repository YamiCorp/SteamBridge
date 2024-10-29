// Copyright 2020-2024 Russ 'trdwll' Treadwell <trdwll.com>. All Rights Reserved.

#pragma once
#include <CoreMinimal.h>

#include "Kismet/BlueprintAsyncActionBase.h"
#include "SteamStructs.h"

#include "SteamUserStatsAsync.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFindLeaderboardDelegate, const FSteamLeaderboard&, Leaderboard);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFindOrCreateLeaderboardDelegate, const FSteamLeaderboard&, Leaderboard);

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
