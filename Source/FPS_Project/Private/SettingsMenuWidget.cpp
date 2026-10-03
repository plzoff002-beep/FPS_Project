// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingsMenuWidget.h"
#include "Components/WidgetSwitcher.h"

void USettingsMenuWidget::UnifiedButtonHandler(EButtonType ButtonType, UObject*) {
	if (OnButtonClicked.IsBound()) {
		switch (ButtonType) {
			case EButtonType::Gameplay:
				SettingsSwitcher->SetActiveWidgetIndex(0);
				break;

			case EButtonType::Graphics:
				SettingsSwitcher->SetActiveWidgetIndex(1);
				break;

			case EButtonType::Audio:
				SettingsSwitcher->SetActiveWidgetIndex(2);
				break;

			default:
				OnButtonClicked.Broadcast(ButtonType, nullptr);
				break;
		}
	}
}

void USettingsMenuWidget::NativeConstruct() {
	Super::NativeConstruct();

	auto BindWidget = [this](UFPSButton* Button) {
		if (!Button) return;

		Button->BindDelegate();
		Button->OnButtonClicked.AddUObject(this, &USettingsMenuWidget::UnifiedButtonHandler);
	};

	BindWidget(BackButton);
	BindWidget(GameplayButton);
	BindWidget(GraphicsButton);
	BindWidget(AudioButton);

	if (SettingsSwitcher) {
		SettingsSwitcher->SetActiveWidgetIndex(0);
	}
}