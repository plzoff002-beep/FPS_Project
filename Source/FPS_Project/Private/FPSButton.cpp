// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSButton.h"

void UFPSButton::SetButtonText(FText NewText) {
	ButtonText->SetText(NewText);
}

void UFPSButton::SetButtonIndex(int32 NewIndex) {
	ButtonIndex = NewIndex;
}

void UFPSButton::HandleInternalClick() {
	if (OnButtonClicked.IsBound()) {
		OnButtonClicked.Broadcast(ButtonType, nullptr);
	}

	if (OnQualityLevelChanged.IsBound()) {
		OnQualityLevelChanged.Broadcast(ButtonIndex);
	}
}

void UFPSButton::BindDelegate() {
	OnClicked.AddDynamic(
		this,
		&UFPSButton::HandleInternalClick
	);
}