// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EditableElementBase.generated.h"


UENUM(BlueprintType)
enum class EditableType : uint8
{
	Boolean,
	Float,
	String,
	Image
};

UCLASS()
class RAILWAYS_API UEditableElementBase : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
	EditableType EditType;

};