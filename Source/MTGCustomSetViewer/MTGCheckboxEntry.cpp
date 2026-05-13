// Fill out your copyright notice in the Description page of Project Settings.


#include "MTGCheckboxEntry.h"

#include "Components/CheckBox.h"
#include "Components/TextBlock.h"

#include "Styling/SlateTypes.h"


void UMTGCheckboxEntry::SetText_Implementation(const FString& InText)
{
	TextBlock->SetText(FText::FromString(InText));
}

void UMTGCheckboxEntry::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	CheckBox->SetCheckedState(bCheckedByDefault ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
	CheckBox->OnCheckStateChanged.AddDynamic(this, &UMTGCheckboxEntry::OnCheckboxToggled);
}

FString UMTGCheckboxEntry::GetText()
{
	return TextBlock->GetText().ToString();
}

void UMTGCheckboxEntry::OnCheckboxToggled(bool bIsChecked)
{
	CheckboxToggled.Broadcast(this, bIsChecked);
}
