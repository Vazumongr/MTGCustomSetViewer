// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "MTGCardDataTypes.h"

#include "MTGCard.generated.h"

class UImage;



UCLASS()
class UMTGCardWidgetData : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY()
	UTexture2D* ImageTexture;

	FString test;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	FMTGCardData CardData;
};


/**
 * 
 */
UCLASS()
class MTGCUSTOMSETVIEWER_API UMTGCard : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:

	virtual void NativeOnInitialized() override;

	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;


	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(BindWidget))
	UImage* CardImage;
	
	UPROPERTY()
	UMTGCardWidgetData* WidgetData;
};