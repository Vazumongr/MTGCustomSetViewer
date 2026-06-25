// Fill out your copyright notice in the Description page of Project Settings.


#include "MTGCard.h"

#include "MTGSetViewer.h"
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
	WidgetData->SetViewer->CardWidgetCreated(this);
	ensure(WidgetData);
}

FReply UMTGCard::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	CardClickedDelegate.Broadcast(this);
	return reply;
}
