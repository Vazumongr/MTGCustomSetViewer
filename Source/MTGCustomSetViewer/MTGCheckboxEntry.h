// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MTGCheckboxEntry.generated.h"

class UCheckBox;
class UTextBlock;

DECLARE_MULTICAST_DELEGATE_TwoParams(FCheckboxToggled, class UMTGCheckboxEntry*, bool);

/**
 * 
 */
UCLASS()
class MTGCUSTOMSETVIEWER_API UMTGCheckboxEntry : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeOnInitialized() override;
	
public:

	FString GetText();

	UFUNCTION()
	void OnCheckboxToggled(bool bIsChecked);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetText(const FString& InText);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCheckedByDefault = true;

	FCheckboxToggled CheckboxToggled;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(BindWidget))
	TObjectPtr<UCheckBox> CheckBox;
};
