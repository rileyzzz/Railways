// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerList.h"
#include "../RailwaysGameInstance.h"
#include "Kismet/GameplayStatics.h"

//void UServerList::PopulateList(TArray<FRailwaysServerData>& ServerData)
//{
//	auto ListItemWidget = List->GetEntryWidgetClass();
//	for (const auto& Data : ServerData)
//	{
//		//auto NewItem = NewObject<ListItemWidget>(ListItemWidget);
//		//UServerListEntry
//		//List->AddItem();
//	}
//}

void UServerList::RefreshServerList()
{
	ListContent->ClearListItems();

	URailwaysGameInstance* Instance = Cast<URailwaysGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (Instance)
	{
		Instance->QueryServerList(this);
	}
}

//void UServerList::PopulateList(const TArray<FRailwaysServerData>& ServerData)
//{
//	auto WidgetClass = ListContent->GetEntryWidgetClass();
//	for (const FRailwaysServerData& Data : ServerData)
//	{
//		//UE_LOG(LogTemp, Log, TEXT("server %s"), *Data.Name);
//		//auto NewItem = Cast<UServerListEntry>(WidgetClass.GetDefaultObject());
//		//NewItem->ServerName->SetText(FText::FromString(Data.Name));
//		auto NewItem = NewObject<UServerListEntry>(this, WidgetClass);
//		NewItem->ServerName->SetText(FText::FromString(Data.Name));
//		ListContent->AddItem(NewItem);
//	}
//}

void UServerList::PopulateList_Implementation(const TArray<FRailwaysServerData>& ServerData)
{
}
