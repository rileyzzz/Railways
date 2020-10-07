// Fill out your copyright notice in the Description page of Project Settings.


#include "RailwaysGameInstance.h"
#include "Kismet/GameplayStatics.h"

void URailwaysGameInstance::Init()
{
	Super::Init();
	//Interface = GetContentSystem();
	Interface = UContentSystemInterface::GetInterface();
	AssimpInterface = NewObject<UAssimpInterface>();
	//FOnlineSubsystemSteam::Get();
	IOnlineSubsystem* SubSystem = IOnlineSubsystem::Get();
	SessionInterface = SubSystem->GetSessionInterface();
	if (SessionInterface.IsValid())
	{
		//IOnlineSession* InterfacePtr = SessionInterface.Get();
		SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &URailwaysGameInstance::OnCreateSessionComplete);
		SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &URailwaysGameInstance::OnDestroySessionComplete);
		SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &URailwaysGameInstance::OnFindSessionsComplete);
		SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &URailwaysGameInstance::OnJoinSessionComplete);
	}
}

void URailwaysGameInstance::Shutdown()
{
	//UE_LOG(LogTemp, Warning, TEXT("Shutdown instance"));
	UContentSystemInterface::ShutdownInterface();
	Interface = nullptr;
	Super::Shutdown();
}

void URailwaysGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if(bWasSuccessful)
	{
		UE_LOG(LogTemp, Log, TEXT("Session created successfully."));
		UGameplayStatics::OpenLevel(GetWorld(), TEXT("CustomWorld"), true, TEXT("?listen"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Session creation failed!"));
	}
}

void URailwaysGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Log, TEXT("Session destroyed."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not destroy session!"));
	}
}

void URailwaysGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("Session find complete."));
	if (bWasSuccessful && SessionSearch.IsValid())
	{
		if (SessionSearch->SearchResults.Num())
		{
			TArray<FRailwaysServerData> ServerData;
			uint32 Index = 0;
			for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
			{
				FRailwaysServerData Data;
				Data.Name = SearchResult.Session.OwningUserName;
				FString MapName;
				if (SearchResult.Session.SessionSettings.Get(SETTING_MAPNAME, MapName))
					Data.Name = MapName;

				Data.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
				Data.CurrentPlayers = Data.MaxPlayers - SearchResult.Session.NumOpenPublicConnections;
				Data.HostUsername = SearchResult.Session.OwningUserName;
				Data.Index = Index++;
				UE_LOG(LogTemp, Warning, TEXT("found session %s"), *Data.Name);
				for (const auto& data : SearchResult.Session.SessionSettings.Settings)
				{
					UE_LOG(LogTemp, Warning, TEXT("Session setting %s = %s"), *data.Key.ToString(), *data.Value.ToString());
				}
				ServerData.Add(Data);
			}
			UE_LOG(LogTemp, Warning, TEXT("found %i sessions"), SessionSearch->SearchResults.Num());
			ActiveServerList->PopulateList(ServerData);
		}
	}
}

void URailwaysGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogTemp, Log, TEXT("Joining session."));
	FString URL;
	if (!SessionInterface->GetResolvedConnectString(SessionName, URL))
		return;
	APlayerController* Controller = GetFirstLocalPlayerController();
	Controller->ClientTravel(URL, ETravelType::TRAVEL_Absolute);
	//switch (Result)
	//{
	//case EOnJoinSessionCompleteResult::Type::Success:
	//	UE_LOG(LogTemp, Log, TEXT("Joined session %s."), *SessionName.ToString());
	//	break;
	//case EOnJoinSessionCompleteResult::Type::AlreadyInSession:
	//	UE_LOG(LogTemp, Warning, TEXT("Already in session %s!"), *SessionName.ToString());
	//	break;
	//default:
	//case EOnJoinSessionCompleteResult::UnknownError:
	//	UE_LOG(LogTemp, Warning, TEXT("Unknown session join error"));
	//}
}

void URailwaysGameInstance::BeginSession(FString ServerName)
{
	UE_LOG(LogTemp, Log, TEXT("Starting server %s"), *ServerName);
	if (SessionInterface.IsValid())
	{
		FOnlineSessionSettings Settings;
		Settings.bIsLANMatch = false;
		Settings.NumPublicConnections = 10;
		Settings.bShouldAdvertise = true;
		Settings.bUsesPresence = true;
		//Settings.Set(SERVER_NAME_SETTINGS_KEY, ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		SessionInterface->CreateSession(0, TEXT("Railways Session"), Settings); //TEXT("Railways Session")
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find online subsystem!"));
	}
}

void URailwaysGameInstance::JoinSession(uint32 Index)
{
	UE_LOG(LogTemp, Warning, TEXT("Attempting to join session %u"), Index);
	if (Index < (uint32)SessionSearch->SearchResults.Num())
	{
		SessionInterface->JoinSession(0, TEXT("Railways Session"), SessionSearch->SearchResults[Index]);
	}
}

void URailwaysGameInstance::QueryServerList(UServerList* ServerList)
{
	ActiveServerList = ServerList;
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (SessionSearch.IsValid())
	{
		SessionSearch->MaxSearchResults = 100000;
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());


		//TArray<FRailwaysServerData> ServerData;
		//for (int i = 0; i < 5; i++)
		//{
		//	FRailwaysServerData Data;
		//	Data.Name = TEXT("Test server");
		//	Data.MaxPlayers = 8;
		//	Data.CurrentPlayers = 1;
		//	Data.HostUsername = TEXT("hosttest");
		//	ServerData.Add(Data);
		//}
		//ActiveServerList->PopulateList(ServerData);
	}
}
