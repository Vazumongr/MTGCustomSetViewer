// Fill out your copyright notice in the Description page of Project Settings.


#include "MTGSetViewer.h"

#include "EditorAssetLibrary.h"
#include "ImageUtils.h"
#include "MTGCard.h"
#include "MTGCheckboxEntry.h"
#include "XmlFile.h"
#include "Components/Image.h"
#include "Components/TileView.h"
#include "Components/VerticalBox.h"
#include "Engine/ObjectLibrary.h"
#include "HAL/FileManagerGeneric.h"
#include "MTGCheckboxEntry.h"


void UMTGSetViewer::NativeConstruct()
{
	Super::NativeConstruct();

	ParseXMLData();
	PopulateSetsDropdown();
	PopulateColorsDropdown();
	PopulateTypesDropdown();

	UE_LOG(LogTemp, Warning, TEXT("Hi"));
}

void UMTGSetViewer::ParseXMLData()
{
	FString setsDirectory = FPaths::ProjectSavedDir() + TEXT("/Sets/");
	FString FileExtension = TEXT("");

	//FString fileName = FPaths::ProjectSavedDir() + "/Sets/MyNewSet/setGlorbo.xml";
	//FString imagesDirectory = FPaths::ProjectSavedDir() + TEXT("/Sets/MyNewSet/setGlorbo-files/");

	TArray<FString> setDirectories;
	IFileManager& fileManager = FFileManagerGeneric::Get();

	fileManager.FindFiles(setDirectories, *(setsDirectory + TEXT("*.*")), false, true);

	for (FString setDir : setDirectories)
	{
		FMTGCardSet cardSet = FMTGCardSet(setDir);
		FString setDataPath = FPaths::ProjectSavedDir() + FString::Printf(TEXT("/Sets/%s/%s.xml"), *setDir, *setDir);
		FString setImagesPath = FPaths::ProjectSavedDir() + FString::Printf(TEXT("/Sets/%s/%s-files/"), *setDir, *setDir);

		UE_LOG(LogTemp, Warning, TEXT("%s"), *setDataPath);

		FString fileText;
		FArchive fileReader;
		bool bResult = FFileHelper::LoadFileToString(fileText, *setDataPath);
		int32 tokenStringIdx = fileText.Find(TOKEN_STRING); 
		
		if (tokenStringIdx != INDEX_NONE)
		{
			fileText.RemoveAt(tokenStringIdx, TOKEN_STRING.Len());
		}
		

		FXmlFile xmlFile = FXmlFile(fileText, EConstructMethod::ConstructFromBuffer);

		FXmlNode* rootNode = xmlFile.GetRootNode();

		if (rootNode == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("%hs"), (bResult ? "good" : "bad"));
			return;
		}

		const FXmlNode* cardsNode = nullptr;

		TArray<FMTGCardData>& cards = cardSet.Cards;

		for (auto* node : rootNode->GetChildrenNodes())
		{
			if (node->GetTag() == CARDS_TAG)
			{
				for (auto* cardNode : node->GetChildrenNodes())
				{
					FMTGCardData cardData = FMTGCardData();
					for (auto* cardDataNode : cardNode->GetChildrenNodes())
					{
						if (!cardDataNode->GetContent().IsEmpty())
						{
							//cardData.Data.Emplace(cardDataNode->GetTag(), cardDataNode->GetContent());
							AddCardData(cardData, cardDataNode->GetTag(), cardDataNode->GetContent());
						}
						else if (!cardDataNode->GetChildrenNodes().IsEmpty())
						{
							for (auto* subData : cardDataNode->GetChildrenNodes())
							{
								//cardData.Data.Emplace(subData->GetTag(), subData->GetContent());
								AddCardData(cardData, subData->GetTag(), subData->GetContent());
							}
						}
						else if (!cardDataNode->GetAttributes().IsEmpty())
						{
							for (const auto subData : cardDataNode->GetAttributes())
							{
								//cardData.Data.Emplace(subData.GetTag(), subData.GetValue());
								AddCardData(cardData, subData.GetTag(), subData.GetValue());
							}
						}
					}
					cards.Add(cardData);
					auto copyFoRbeakpoint = cardData;
				}
				break;
			}
		}

		auto copy = cards;

		for (auto card : cards)
		{
			UMTGCardWidgetData* widgetData = NewObject<UMTGCardWidgetData>();
			widgetData->CardData = card;
			widgetData->test = TEXT("isahjdjlkashdjkasd");
			FString cardName = card.GetContentForTag(TEXT("name"));
			FString imagePath = setImagesPath + cardName + TEXT(".png");

			FImage cardImage;
			FImageUtils::LoadImage(*imagePath, cardImage);

			UTexture2D* cardTexture = FImageUtils::CreateTexture2DFromImage(cardImage);

			widgetData->ImageTexture = cardTexture;

			auto imageSize = FVector2D(750,1030) * EntryScale;
			CardTileView->SetEntryHeight(imageSize.Y);
			CardTileView->SetEntryWidth(imageSize.X);
			widgetData->test = TEXT("dahsjdhaslkdj;");
			CardTileView->AddItem(widgetData);
		}
		Sets.Add(cardSet);
		Filter.AllowedSets.Add(cardSet.SetName);
		Filter.ExistingSets.Add(cardSet.SetName);
	}
	UE_LOG(LogTemp, Warning, TEXT("Hi"));
}

void UMTGSetViewer::PopulateTypesDropdown()
{
	TypesDropDown->ClearChildren();
	for (auto type : Filter.ExistingTypes)
	{
		UMTGCheckboxEntry* checkboxEntry = CreateWidget<UMTGCheckboxEntry>(GetOwningPlayer(), CheckboxEntryClass);
		checkboxEntry->SetText(type);
		checkboxEntry->CheckboxToggled.AddUObject(this, &UMTGSetViewer::OnTypeToggled);
		TypesDropDown->AddChildToVerticalBox(checkboxEntry);
	}
}

void UMTGSetViewer::PopulateColorsDropdown()
{
	ColorsDropDown->ClearChildren();
	for (auto color : Filter.ExistingColors)
	{
		UMTGCheckboxEntry* checkboxEntry = CreateWidget<UMTGCheckboxEntry>(GetOwningPlayer(), CheckboxEntryClass);
		checkboxEntry->SetText(color);
		checkboxEntry->CheckboxToggled.AddUObject(this, &UMTGSetViewer::OnColorToggled);
		ColorsDropDown->AddChildToVerticalBox(checkboxEntry);
	}
}

void UMTGSetViewer::PopulateSetsDropdown()
{
	SetDropDown->ClearChildren();
	for (auto set : Sets)
	{
		UMTGCheckboxEntry* checkboxEntry = CreateWidget<UMTGCheckboxEntry>(GetOwningPlayer(), CheckboxEntryClass);
		checkboxEntry->SetText(set.SetName);
		checkboxEntry->CheckboxToggled.AddUObject(this, &UMTGSetViewer::OnSetToggled);
		SetDropDown->AddChildToVerticalBox(checkboxEntry);
	}
}

void UMTGSetViewer::RebuildView()
{
	CardTileView->ClearListItems();
	for (auto& set : Sets)
	{
		if (set.bActive == false)
		{
			continue;
		}
		for (auto card : set.Cards)
		{
			if (IsCardFiltered(card))
			{
				continue;
			}
			FString setDataPath = FPaths::ProjectSavedDir() + FString::Printf(TEXT("/Sets/%s/%s.xml"), *set.SetName, *set.SetName);
			FString setImagesPath = FPaths::ProjectSavedDir() + FString::Printf(TEXT("/Sets/%s/%s-files/"), *set.SetName, *set.SetName);
			UMTGCardWidgetData* widgetData = NewObject<UMTGCardWidgetData>();
			widgetData->CardData = card;
			widgetData->test = TEXT("isahjdjlkashdjkasd");
			FString cardName = card.GetContentForTag(TEXT("name"));
			FString imagePath = setImagesPath + cardName + TEXT(".png");

			FImage cardImage;
			FImageUtils::LoadImage(*imagePath, cardImage);

			UTexture2D* cardTexture = FImageUtils::CreateTexture2DFromImage(cardImage);

			widgetData->ImageTexture = cardTexture;

			auto imageSize = FVector2D(750,1030) * EntryScale;
			CardTileView->SetEntryHeight(imageSize.Y);
			CardTileView->SetEntryWidth(imageSize.X);
			widgetData->test = TEXT("dahsjdhaslkdj;");
			CardTileView->AddItem(widgetData);
		}
	}
}

void UMTGSetViewer::AddCardData(FMTGCardData& CardData, FString Name, FString Value)
{
	CardData.Data.Emplace(Name, Value);
	if (Name.Equals(TEXT("colors"), ESearchCase::IgnoreCase))
	{
		for (TCHAR Ch : Value)
		{
			Filter.ExistingColors.Add(FString(1, &Ch));
		}
		for (auto color : Value)
		{
			FString coolString = FString(&color);
			FString coolerString = FString(1, &color);
			Filter.ExistingColors.Add(coolerString);
			Filter.AllowedColors.Add(coolerString);
			bool be = Filter.AllowedColors.Contains(coolerString);
			UE_LOG(LogTemp, Warning, TEXT("hi"));
		}
	}
	else if (Name.Equals(TEXT("rarity"), ESearchCase::IgnoreCase))
	{
		Filter.ExistingRarities.Add(Value);
		Filter.AllowedRarities.Add(Value);
	}
	else if (Name.Equals(TEXT("type"), ESearchCase::IgnoreCase))
	{
		Filter.ExistingTypes.Add(Value);
		Filter.AllowedTypes.Add(Value);
	}
}

bool UMTGSetViewer::IsCardFiltered(FMTGCardData& InCard)
{
	FString colors = InCard.GetContentForTag(TEXT("COLORS"));
	FString rarity = InCard.GetContentForTag(TEXT("RARITY"));
	FString type = InCard.GetContentForTag(TEXT("TYPE"));

	if (!Filter.AllowedRarities.Contains(rarity))
	{
		return true;
	}
	if (!Filter.AllowedTypes.Contains(type))
	{
		return true;
	}

	for (auto color : colors)
	{
		FString coolString = FString(&color);
		FString coolerString = FString(1, &color);
		bool result = Filter.AllowedColors.Contains(coolerString);
		if (result)
		{
			return false;
		}
		return true;
	}
	

	for (auto color : colors)
	{
		FString col = FString(1, &color);
		bool result = Filter.AllowedColors.Contains(&color);
		if (result)
		{
			return false;
		}
		return true;
	}
	return false;
}


void UMTGSetViewer::NotifyCardClicked_Implementation(UMTGCard* ClickedCard)
{
	
}



void UMTGSetViewer::OnSetToggled(UMTGCheckboxEntry* InCheckbox, bool bIsChecked)
{
	FString toggledSet = InCheckbox->GetText();
	FMTGCardSet* set = Sets.FindByPredicate([toggledSet](FMTGCardSet& Set)
	{
		return toggledSet == Set.SetName;
	});

	if (set != nullptr)
	{
		if (bIsChecked)
		{
			set->bActive = true;
		}
		else
		{
			set->bActive = false;
		}
	}

	RebuildView();
}

void UMTGSetViewer::OnColorToggled(UMTGCheckboxEntry* InCheckbox, bool bIsChecked)
{
	FString toggledColor = InCheckbox->GetText();
	if (bIsChecked)
	{
		Filter.AllowedColors.Add(toggledColor);
	}
	else
	{
		Filter.AllowedColors.Remove(toggledColor);
	}
	RebuildView();
}

void UMTGSetViewer::OnTypeToggled(UMTGCheckboxEntry* InCheckbox, bool bIsChecked)
{
	FString toggledType = InCheckbox->GetText();
	if (bIsChecked)
	{
		Filter.AllowedTypes.Add(toggledType);
	}
	else
	{
		Filter.AllowedTypes.Remove(toggledType);
	}
	RebuildView();
}
