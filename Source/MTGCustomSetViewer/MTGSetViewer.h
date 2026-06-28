// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MTGCardDataTypes.h"
#include "MTGSetViewer.generated.h"

class IHttpResponse;
class IHttpRequest;
class USlider;
class UEditableTextBox;
class UComboBoxString;
class UMTGCard;
class UMTGCardPreview;
class UMTGCardWidgetData;
class UMTGCheckboxEntry;
class UTileView;
class UVerticalBox;

USTRUCT(BlueprintType)
struct FFilterSettings
{
	GENERATED_BODY()

	FString SearchField;
	FString SearchText;

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

	void CardWidgetCreated(UMTGCard* InCard);

	
	UFUNCTION()
	void OnCardClicked(class UMTGCard* ClickedCard);

private:
	virtual void NativeConstruct() override;

	void ParseXMLData();
	void PopulateSetsDropdown();
	void PopulateColorsDropdown();
	void PopulateTypesDropdown();
	void PopulateSearchFields();
	void RebuildView();
	void HandleImageRequest(TSharedPtr<IHttpRequest> HttpRequest, TSharedPtr<IHttpResponse> HttpResponse, bool bArg, TWeakObjectPtr<UMTGCardWidgetData> WidgetData);
	void GenerateWidgetData();

	void AddCardData(FMTGCardData& CardData, FString Name, FString Value);

	bool IsCardFiltered(FMTGCardData& InCard);
	
	UFUNCTION()
	void SearchFieldChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	
	UFUNCTION()
	void SearchTextUpdated(const FText& Text);
	
	UFUNCTION()
	void CardSizeChanged(float Value);

protected:
	UFUNCTION(BlueprintNativeEvent)
	void NotifyCardClicked(UMTGCard* ClickedCard);

	UPROPERTY()
	TArray<UMTGCardWidgetData*> WidgetDataArray;

	UPROPERTY()
	TSet<FMTGCardData> Cards;
	
	FString SetsDirectory;
	
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

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta=(BindWidget))
	TObjectPtr<UComboBoxString> SearchFields;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta=(BindWidget))
	TObjectPtr<UEditableTextBox> SearchTextBox;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	TSet<FString> DiscoveredAttributes;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UMTGCardPreview> CardPreviewClass;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(BindWidget))
	TObjectPtr<USlider> CardSizeSlider;
};
