// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MTGCardDataTypes.h"
#include "MTGSetViewer.generated.h"

class UVerticalBox;
class UTileView;
class UMTGCard;
class UMTGCheckboxEntry;

USTRUCT(BlueprintType)
struct FFilterSettings
{
	GENERATED_BODY()

	TSet<FString> AllowedSets;
	TSet<FString> AllowedColors;
	TSet<FString> AllowedRarities;
	TSet<FString> AllowedTypes;

	TSet<FString> ExistingSets;
	TSet<FString> ExistingColors;
	TSet<FString> ExistingRarities;
	TSet<FString> ExistingTypes;
};

/**
 * 
 */
UCLASS(Config=Game)
class MTGCUSTOMSETVIEWER_API UMTGSetViewer : public UUserWidget
{
	GENERATED_BODY()

public:

	void OnSetToggled(UMTGCheckboxEntry* InCheckbox, bool bIsChecked);
	void OnColorToggled(UMTGCheckboxEntry* InCheckbox, bool bIsChecked);
	void OnTypeToggled(UMTGCheckboxEntry* InCheckbox, bool bIsChecked);

private:

	virtual void NativeConstruct() override;

	void ParseXMLData();
	void PopulateSetsDropdown();
	void PopulateColorsDropdown();
	void PopulateTypesDropdown();
	void RebuildView();

	void AddCardData(FMTGCardData& CardData, FString Name, FString Value);

	bool IsCardFiltered(FMTGCardData& InCard);

protected:
	UFUNCTION(BlueprintNativeEvent)
	void NotifyCardClicked(UMTGCard* ClickedCard);
	
	UPROPERTY(Config, BlueprintReadWrite)
	FString MSESetLocation = TEXT("Test");

	UPROPERTY(Config, BlueprintReadWrite)
	FString TOKEN_STRING;

	UPROPERTY(Config, BlueprintReadWRite)
	FString CARDS_TAG;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(BindWidget))
	UTileView* CardTileView;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UMTGCard> CardClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float EntryScale = .5f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FFilePath ImageAssetsPath;

	UPROPERTY(EditAnywhere)
	UTexture2D* testTexture;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	TArray<FMTGCardSet> Sets;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(BindWidget))
	UVerticalBox* SetDropDown;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(BindWidget))
	UVerticalBox* ColorsDropDown;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(BindWidget))
	UVerticalBox* TypesDropDown;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UMTGCheckboxEntry> CheckboxEntryClass;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FFilterSettings Filter;
};
