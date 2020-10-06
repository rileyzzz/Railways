// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ListView.h"
#include "ServerListEntry.h"
#include "ServerList.generated.h"


UCLASS()
class RAILWAYS_API UServerList : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UListView* ListContent;

	UFUNCTION(BlueprintCallable, Category = "Server List")
	void RefreshServerList();

	UFUNCTION(BlueprintNativeEvent, Category="Server List")
	void PopulateList(const TArray<FRailwaysServerData>& ServerData);

	void PopulateList_Implementation(const TArray<FRailwaysServerData>& ServerData);
};
