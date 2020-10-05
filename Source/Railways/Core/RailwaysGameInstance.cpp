// Fill out your copyright notice in the Description page of Project Settings.


#include "RailwaysGameInstance.h"

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
		UE_LOG(LogTemp, Log, TEXT("Session %s created successfully."), *SessionName.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Session not created successfully!"));
	}
}

void URailwaysGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Log, TEXT("Session %s destroyed."), *SessionName.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not destroy session!"));
	}
}

void URailwaysGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{

}

void URailwaysGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	switch (Result)
	{
	case EOnJoinSessionCompleteResult::Type::Success:
		UE_LOG(LogTemp, Log, TEXT("Joined session %s."), *SessionName.ToString());
		break;
	case EOnJoinSessionCompleteResult::Type::AlreadyInSession:
		UE_LOG(LogTemp, Warning, TEXT("Already in session %s!"), *SessionName.ToString());
		break;
	default:
	case EOnJoinSessionCompleteResult::UnknownError:
		UE_LOG(LogTemp, Warning, TEXT("Unknown session join error"));
	}
}

void URailwaysGameInstance::BeginSession(FString ServerName)
{
	UE_LOG(LogTemp, Log, TEXT("Starting server %s"), *ServerName);
	if (SessionInterface.IsValid())
	{
		FOnlineSessionSettings Settings;
		Settings.bIsLANMatch = false;
		Settings.NumPublicConnections = 2;
		Settings.bShouldAdvertise = true;
		Settings.bUsesPresence = true;
		//Settings.Set(SERVER_NAME_SETTINGS_KEY, ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		if (SessionInterface->CreateSession(0, *ServerName, Settings)) //TEXT("Railways Session")
		{
			UE_LOG(LogTemp, Log, TEXT("Creating session..."));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to create session!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find online subsystem!"));
	}
}
