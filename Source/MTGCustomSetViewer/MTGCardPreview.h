// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MTGCardPreview.generated.h"

class UMTGCard;
/**
 * 
 */
UCLASS()
class MTGCUSTOMSETVIEWER_API UMTGCardPreview : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void InitCardData(UMTGCard* InCard);
};
