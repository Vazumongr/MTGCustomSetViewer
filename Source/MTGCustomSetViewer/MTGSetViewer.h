// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MTGSetViewer.generated.h"

class UTileView;
class UMTGCard;

/**
 * 
 */
UCLASS(Config=Game)
class MTGCUSTOMSETVIEWER_API UMTGSetViewer : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

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
};
