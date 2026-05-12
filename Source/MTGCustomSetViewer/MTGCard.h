// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "MTGCard.generated.h"

class UImage;

USTRUCT()
struct FMTGCardAttribute
{
	GENERATED_BODY()

	FMTGCardAttribute() = default;

	FMTGCardAttribute(FString InTag, FString InContent)
		: Tag(InTag), Content(InContent)
	{
		
	}
	
	FString Tag;
	FString Content;
};

USTRUCT(BlueprintType)
struct FMTGCardData
{
	GENERATED_BODY();

	FString GetContentForTag(FString InTag)
	{
		FMTGCardAttribute* foundAttr = Data.FindByPredicate([InTag](const FMTGCardAttribute& attr)
		{
			return attr.Tag.Equals(InTag, ESearchCase::IgnoreCase);
		});

		if (foundAttr == nullptr)
		{
			return TEXT("");
		}
		return foundAttr->Content;
	}

	UPROPERTY()
	TArray<FMTGCardAttribute> Data;
};

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
