// Fill out your copyright notice in the Description page of Project Settings.


#include "MTGSetViewer.h"

#include "EditorAssetLibrary.h"
#include "ImageUtils.h"
#include "MTGCard.h"
#include "XmlFile.h"
#include "Components/Image.h"
#include "Components/TileView.h"
#include "Engine/ObjectLibrary.h"
#include "HAL/FileManagerGeneric.h"


void UMTGSetViewer::NativeConstruct()
{
	Super::NativeConstruct();

	FString setsDirectory = FPaths::ProjectSavedDir() + TEXT("/Sets/");
	FString FileExtension = TEXT("");

	FString fileName = FPaths::ProjectSavedDir() + "/Sets/MyNewSet/setGlorbo.xml";
	FString imagesDirectory = FPaths::ProjectSavedDir() + TEXT("/Sets/MyNewSet/setGlorbo-files/");

	TArray<FString> setDirectories;
	IFileManager& fileManager = FFileManagerGeneric::Get();

	fileManager.FindFiles(setDirectories, *(setsDirectory + TEXT("*.*")), false, true);

	for (FString setDir : setDirectories)
	{
		FString setDataPath = FPaths::ProjectSavedDir() + FString::Printf(TEXT("/Sets/%s/%s.xml"), *setDir, *setDir);
		FString setImagesPath = FPaths::ProjectSavedDir() + FString::Printf(TEXT("/Sets/%s/%s-files/"), *setDir, *setDir);
		FString te = "hello";
	}

	UE_LOG(LogTemp, Warning, TEXT("%s"), *fileName);

	FString fileText;
	FArchive fileReader;
	bool bResult = FFileHelper::LoadFileToString(fileText, *fileName);
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

	TArray<FMTGCardData> cards;

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
						cardData.Data.Emplace(cardDataNode->GetTag(), cardDataNode->GetContent());
					}
					else if (!cardDataNode->GetChildrenNodes().IsEmpty())
					{
						for (auto* subData : cardDataNode->GetChildrenNodes())
						{
							cardData.Data.Emplace(subData->GetTag(), subData->GetContent());
						}
					}
					else if (!cardDataNode->GetAttributes().IsEmpty())
					{
						for (const auto subData : cardDataNode->GetAttributes())
						{
							cardData.Data.Emplace(subData.GetTag(), subData.GetValue());
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
		FString imagePath = imagesDirectory + cardName + TEXT(".png");

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

	
	
	UE_LOG(LogTemp, Warning, TEXT("%hs"), (bResult ? "good" : "bad"));
}

void UMTGSetViewer::NotifyCardClicked_Implementation(UMTGCard* ClickedCard)
{
	
}
