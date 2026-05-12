// Fill out your copyright notice in the Description page of Project Settings.


#include "MTGCard.h"

#include "Components/Image.h"

void UMTGCard::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UMTGCard::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMTGCard::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UMTGCard::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	WidgetData = Cast<UMTGCardWidgetData>(ListItemObject);
	CardImage->SetBrushResourceObject(WidgetData->ImageTexture);
	ensure(WidgetData);
}
