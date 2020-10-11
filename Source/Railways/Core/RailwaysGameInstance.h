// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AssimpInterface.h"
//#include "ContentSystemInterface.h"
#include "ContentSystem/ContentSystemInterface.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystemSteam.h"
#include "UI/ServerList.h"
#include "GameFramework/PlayerState.h"
#include "RailwaysGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FSteamID
{
	GENERATED_BODY()
public:
	UPROPERTY()
	uint64 ID;
	FSteamID() : ID(0) { }
	FSteamID(uint64 InID) : ID(InID) { }
};

UCLASS()
class RAILWAYS_API URailwaysGameInstance : public UGameInstance
{
	GENERATED_BODY()
private:
	UPROPERTY()
	UContentSystemInterface* Interface;

	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	UPROPERTY()
	UServerList* ActiveServerList;
public:
	UPROPERTY()
	UAssimpInterface* AssimpInterface;

	virtual void Init() override;
	virtual void Shutdown() override;

	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	void BeginSession(FString ServerName);

	//UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	void JoinSession(uint32 Index = 0);

	//UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	void QueryServerList(UServerList* ServerList);


	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	FSteamID getSteamID();

	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	UTexture2D* getPlayerSteamAvatar(FSteamID SteamID, UTexture2D* Default);
};
