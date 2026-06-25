// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "MTGCardDataTypes.generated.h"


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

	FString GetContentForTag(FString InTag) const
	{
		const FMTGCardAttribute* foundAttr = Data.FindByPredicate([InTag](const FMTGCardAttribute& attr)
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

USTRUCT(BlueprintType)
struct FMTGCardSet
{
	GENERATED_BODY()

	FMTGCardSet() = default;
	FMTGCardSet(FString InName)
		: SetName(InName)
	{}
	FMTGCardSet(const FMTGCardSet& InSet)
		: SetName(InSet.SetName), Cards(InSet.Cards)
	{}

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	FString SetName;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	TArray<FMTGCardData> Cards;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	bool bActive = true;
};