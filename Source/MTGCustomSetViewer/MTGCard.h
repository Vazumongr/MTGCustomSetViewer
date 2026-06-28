// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "MTGCardDataTypes.h"

#include "MTGCard.generated.h"

class UImage;
class UMTGSetViewer;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCardClickedDelegate, class UMTGCard*, ClickedCard);



UCLASS()
class UMTGCardWidgetData : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY()
	UTexture* ImageTexture;

	FString test;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	FMTGCardData CardData;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UMTGSetViewer* SetViewer;
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

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	void ForceRefreshImage();

	UPROPERTY(BlueprintAssignable)
	FCardClickedDelegate CardClickedDelegate;


	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(BindWidget))
	UImage* CardImage;
	
	UPROPERTY()
	UMTGCardWidgetData* WidgetData;
};