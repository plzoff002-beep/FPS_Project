// Fill out your copyright notice in the Description page of Project Settings.


#include "GraphicsOptionRow.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetTree.h"
#include "FPSButton.h"

void UGraphicsOptionRow::UnifiedButtonHandler(int32 Index) {
	if (OnQualityLevelChanged.IsBound()) {
		OnQualityLevelChanged.Broadcast(Index);
	}
}

void UGraphicsOptionRow::SynchronizeProperties() {
	Super::SynchronizeProperties();

	if (TextLabel) {
		TextLabel->SetText(SettingName);
	}
}

void UGraphicsOptionRow::NativeConstruct() {
	Super::NativeConstruct();

	TArray<UWidget*> FoundButtons;
	WidgetTree->GetAllWidgets(FoundButtons);

	int32 Counter = 0;

	for (UWidget* Widget : FoundButtons) {
		if (!Widget) continue;

		if (UFPSButton* Button = Cast<UFPSButton>(Widget)) {
			Button->BindDelegate();
			Button->SetButtonIndex(Counter++);
			Button->OnQualityLevelChanged.AddUObject(this, &UGraphicsOptionRow::UnifiedButtonHandler);
		}
	}
}