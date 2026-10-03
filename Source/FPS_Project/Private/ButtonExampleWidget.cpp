// Fill out your copyright notice in the Description page of Project Settings.


#include "ButtonExampleWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UButtonExampleWidget::SetButtonText(FText NewText) {
	ButtonText = NewText;

	if (TextBlock) {
		TextBlock->SetText(ButtonText);
	}
}

void UButtonExampleWidget::SetButtonType(EButtonType NewType) {
	ButtonType = NewType;
}

void UButtonExampleWidget::UnifiedButtonHandler() {
	if (OnButtonClicked.IsBound()) {
		OnButtonClicked.Broadcast(ButtonType, nullptr);
	}
}

void UButtonExampleWidget::SynchronizeProperties() {
	if (TextBlock) {
		TextBlock->SetText(ButtonText);
	}
}

void UButtonExampleWidget::NativeConstruct() {
	Super::NativeConstruct();

	if (Button) {
		Button->OnClicked.AddDynamic(this, &UButtonExampleWidget::UnifiedButtonHandler);
	}
}