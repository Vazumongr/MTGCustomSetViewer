// Fill out your copyright notice in the Description page of Project Settings.


#include "MTGSetViewer.h"

#include "HttpModule.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
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
#include "Components/ComboBoxString.h"
#include "Components/EditableTextBox.h"
#include "MTGCardPreview.h"
#include "Components/Slider.h"
#include "Engine/Texture2DDynamic.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

void UMTGSetViewer::NativeConstruct()
{
	Super::NativeConstruct();

	ParseXMLData();
	PopulateSetsDropdown();
	PopulateColorsDropdown();
	PopulateTypesDropdown();
	PopulateSearchFields();

	SearchTextBox->OnTextChanged.AddDynamic(this, &UMTGSetViewer::SearchTextUpdated);
	CardSizeSlider->OnValueChanged.AddDynamic(this, &UMTGSetViewer::CardSizeChanged);

	UE_LOG(LogTemp, Warning, TEXT("Hi"));
}

void UMTGSetViewer::ParseXMLData()
{
// #if WITH_EDITOR
// 	FString setsDirectory = FPaths::ProjectSavedDir() + TEXT("/Sets/");
// #else
// 	FString setsDirectory = FString::Printf(TEXT("../../../")) + TEXT("/Sets/");
// #
#if WITH_EDITOR
	SetsDirectory = FPaths::ProjectSavedDir() + TEXT("/Sets/");
#else
	SetsDirectory = FString::Printf(TEXT("../../../")) + TEXT("/Sets/");
#endif
	UE_LOG(LogTemp, Warning, TEXT("%s"), *SetsDirectory);
	FString FileExtension = TEXT("");

	TArray<FString> setDirectories;
	IFileManager& fileManager = FFileManagerGeneric::Get();

	fileManager.FindFiles(setDirectories, *(SetsDirectory + TEXT("*.*")), false, true);

	for (FString setDir : setDirectories)
	{
		FMTGCardSet cardSet = FMTGCardSet(setDir);
		FString setDataPath = SetsDirectory + FString::Printf(TEXT("/%s/%s.xml"), *setDir, *setDir);
		FString setImagesPath = SetsDirectory + FString::Printf(TEXT("/%s/%s-files/"), *setDir, *setDir);

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

		TArray<FMTGCardData>& cards = cardSet.Cards;
		FString duplicateCardsString = FString(TEXT(""));

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
							AddCardData(cardData, cardDataNode->GetTag(), cardDataNode->GetContent());
						}
						if (!cardDataNode->GetChildrenNodes().IsEmpty())
						{
							for (auto* subData : cardDataNode->GetChildrenNodes())
							{
								AddCardData(cardData, subData->GetTag(), subData->GetContent());
							}
						}
						if (!cardDataNode->GetAttributes().IsEmpty())
						{
							for (const auto subData : cardDataNode->GetAttributes())
							{
								AddCardData(cardData, subData.GetTag(), subData.GetValue());
							}
						}
					}
					if (Cards.Contains(cardData))
					{
						duplicateCardsString += cardData.GetContentForTag(TEXT("name")) + TEXT(" ");
						continue;
					}
					Cards.Add(cardData);
					cards.Add(cardData);
				}
				break;
			}
		}
		
		if (!duplicateCardsString.IsEmpty())
		{
			UE_LOG(LogTemp, Warning, TEXT("Duplicate cards found: %s"), *duplicateCardsString);
		}
		
		Sets.Add(cardSet);
		Filter.AllowedSets.Add(cardSet.SetName);
		Filter.ExistingSets.Add(cardSet.SetName);
	}
	GenerateWidgetData();
	RebuildView();
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

void UMTGSetViewer::PopulateSearchFields()
{
	SearchFields->ClearOptions();
	SearchFields->AddOption(TEXT("All Fields"));
	SearchFields->SetSelectedOption(TEXT("All Fields"));
	Filter.SearchField = TEXT("All Fields");
	for (FString attribute : DiscoveredAttributes)
	{
		SearchFields->AddOption(attribute);
	}
	SearchFields->OnSelectionChanged.AddDynamic(this, &UMTGSetViewer::SearchFieldChanged);
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

void UMTGSetViewer::GenerateWidgetData()
{
	for (auto& set : Sets)
	{
		for (auto card : set.Cards)
		{
			FString setDataPath = SetsDirectory + FString::Printf(TEXT("/%s/%s.xml"), *set.SetName, *set.SetName);
			FString setImagesPath = SetsDirectory + FString::Printf(TEXT("/%s/%s-files/"), *set.SetName, *set.SetName);
			UMTGCardWidgetData* widgetData = NewObject<UMTGCardWidgetData>();
			widgetData->CardData = card;
			widgetData->test = TEXT("isahjdjlkashdjkasd");
			widgetData->SetViewer = this;
			FString cardName = card.GetContentForTag(TEXT("name"));
			FString imagePathPng = setImagesPath + cardName + TEXT(".png");
			FString imagePathJpg = setImagesPath + cardName + TEXT(".jpg");

			auto imageSize = FVector2D(750,1030) * EntryScale;
			CardTileView->SetEntryHeight(imageSize.Y);
			CardTileView->SetEntryWidth(imageSize.X);
			widgetData->test = TEXT("dahsjdhaslkdj;");
			WidgetDataArray.Add(widgetData);
			
			FImage cardImage;
			// Check PNG first, fallback to JPG if PNG fails.
			if (!FImageUtils::LoadImage(*imagePathPng, cardImage))
			{
				// If JPG fails, check for picURL
				if (!FImageUtils::LoadImage(*imagePathJpg, cardImage))
				{
					FString picURL = card.GetContentForTag(TEXT("picURL"));
					TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

					HttpRequest->OnProcessRequestComplete().BindUObject(this, &UMTGSetViewer::HandleImageRequest, TWeakObjectPtr<UMTGCardWidgetData>(widgetData));
					HttpRequest->SetURL(picURL);
					HttpRequest->SetVerb(TEXT("GET"));
					HttpRequest->ProcessRequest();
					continue;
				}
			}

			UTexture2D* cardTexture = FImageUtils::CreateTexture2DFromImage(cardImage);
			widgetData->ImageTexture = cardTexture;
		}
	}
}

void UMTGSetViewer::RebuildView()
{
	CardTileView->ClearListItems();
	TArray<UMTGCardWidgetData*> widgetDataArray;
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
			UMTGCardWidgetData** widgetData = WidgetDataArray.FindByPredicate([card](const auto& widgetData)
			{
				return widgetData->CardData == card;
			});
			check(widgetData)
			widgetDataArray.Add(*widgetData);
		}
	}
	widgetDataArray.Sort([](const UMTGCardWidgetData& cardOne, const UMTGCardWidgetData& cardTwo)
	{
		return cardOne.CardData.GetContentForTag(TEXT("name")) > cardTwo.CardData.GetContentForTag(TEXT("name"));
	});
	CardTileView->SetListItems(widgetDataArray);
}

void UMTGSetViewer::HandleImageRequest(TSharedPtr<IHttpRequest> HttpRequest, TSharedPtr<IHttpResponse> HttpResponse, bool bSucceeded, TWeakObjectPtr<UMTGCardWidgetData> WidgetData)
{
	if ( bSucceeded && HttpResponse.IsValid() && EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()) &&
		 HttpResponse->GetContentLength() > 0 && HttpResponse->GetContent().Num() > 0 )
	{
		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		TSharedPtr<IImageWrapper> ImageWrappers[3] =
		{
			ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG),
			ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG),
			ImageWrapperModule.CreateImageWrapper(EImageFormat::BMP),
		};

		for ( auto ImageWrapper : ImageWrappers )
		{
			if ( ImageWrapper.IsValid() && 
				 ImageWrapper->SetCompressed(HttpResponse->GetContent().GetData(), HttpResponse->GetContent().Num()) &&
				 ImageWrapper->GetWidth() <= TNumericLimits<int32>::Max() && 
				 ImageWrapper->GetHeight() <= TNumericLimits<int32>::Max())
			{
				
				TArray64<uint8> RawData;
				const ERGBFormat InFormat = ERGBFormat::BGRA;
				if ( ImageWrapper->GetRaw(InFormat, 8, RawData) )
				{					
					if ( UTexture2DDynamic* Texture = UTexture2DDynamic::Create(static_cast<int32>(ImageWrapper->GetWidth()), static_cast<int32>(ImageWrapper->GetHeight())) )
					{
						Texture->SRGB = true;
						Texture->UpdateResource();

						FTexture2DDynamicResource* TextureResource = static_cast<FTexture2DDynamicResource*>(Texture->GetResource());
						if (TextureResource)
						{
							ENQUEUE_RENDER_COMMAND(FWriteRawDataToTexture)(
								[TextureResource, RawData = MoveTemp(RawData)](FRHICommandListImmediate& RHICmdList)
								{
									TextureResource->WriteRawToTexture_RenderThread(RawData);
								});
						}
						
						if (WidgetData.IsValid())
						{
							WidgetData->ImageTexture = Texture;
						}
						return;
					}
				}
			}
		}
	}
}

void UMTGSetViewer::AddCardData(FMTGCardData& CardData, FString Name, FString Value)
{
	Value = Value.Replace(TEXT("&apos;"), TEXT("'"));
	CardData.Data.Emplace(Name, Value);
	DiscoveredAttributes.Add(Name);
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

	if (!Filter.SearchText.IsEmpty())
	{
		if (!Filter.SearchField.Equals(TEXT("All Fields"), ESearchCase::IgnoreCase))
		{
			FString content = InCard.GetContentForTag(Filter.SearchField);
			return !content.Contains(Filter.SearchText, ESearchCase::IgnoreCase);
		}
		
		for (auto attr : InCard.Data)
		{
			FString test = TEXT("difuk");
			FString es = TEXT("g");
			bool be = test.Contains(es, ESearchCase::IgnoreCase);
			bool bebebe = attr.Content.Contains(Filter.SearchText, ESearchCase::IgnoreCase);
			FString mystr = FString::Printf(TEXT("%s %s %s"), *attr.Content, *Filter.SearchText, bebebe ? TEXT("true") : TEXT("false"));
			UE_LOG(LogTemp, Warning, TEXT("%s"), *mystr);
			if (attr.Content.Contains(Filter.SearchText, ESearchCase::IgnoreCase))
			{
				UE_LOG(LogTemp, Warning, TEXT("%s contains %s!!!"), *attr.Content, *Filter.SearchText);
				return false;
			}
		}
		return true;
	}

	// Empty rarity means no rarity, ergo let it through.
	if (!Filter.AllowedRarities.Contains(rarity) && rarity != TEXT(""))
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
	
	return false;
}

void UMTGSetViewer::SearchFieldChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	Filter.SearchField = SelectedItem;
	RebuildView();
}

void UMTGSetViewer::SearchTextUpdated(const FText& Text)
{
	Filter.SearchText = Text.ToString();
	RebuildView();
}

void UMTGSetViewer::CardSizeChanged(float Value)
{
	EntryScale = Value;
	auto imageSize = FVector2D(750,1030) * EntryScale;
	CardTileView->SetEntryHeight(imageSize.Y);
	CardTileView->SetEntryWidth(imageSize.X);
	CardTileView->RequestRefresh();
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

void UMTGSetViewer::CardWidgetCreated(UMTGCard* InCard)
{
	if (InCard->CardClickedDelegate.IsAlreadyBound(this, &UMTGSetViewer::OnCardClicked))
	{
		return;
	}
	InCard->CardClickedDelegate.AddDynamic(this, &UMTGSetViewer::OnCardClicked);
}

void UMTGSetViewer::OnCardClicked(class UMTGCard* ClickedCard)
{
	UMTGCardPreview* cardPreview = CreateWidget<UMTGCardPreview>(GetOwningPlayer(), CardPreviewClass);
	cardPreview->InitCardData(ClickedCard);
	cardPreview->AddToViewport();
}
