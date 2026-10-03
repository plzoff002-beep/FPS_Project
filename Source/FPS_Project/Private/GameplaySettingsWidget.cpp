// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplaySettingsWidget.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "CharacterTypes.h"
#include "CharacterInterface.h"
#include "Components/CheckBox.h"
#include "FPSButton.h"
#include "LocalGlobalSettings.h"

ULocalGlobalSettings* UGameplaySettingsWidget::GetLocalGlobalSettings() {
	APlayerController* PC = Cast<APlayerController>(GetOwningPlayer());
	if (!PC) return nullptr;

	return PC->GetLocalPlayer()->GetSubsystem<ULocalGlobalSettings>();
}

void UGameplaySettingsWidget::HandleSensivityChanged(float NewValue) {
	if (MouseSensivitySlider && MouseSensivityValue) {
		MouseSensivitySlider->SetValue(NewValue);
		MouseSensivityValue->SetText(FText::AsNumber(NewValue));

		if (ULocalGlobalSettings* GlobalSettings = GetLocalGlobalSettings()) {
			GlobalSettings->SetAndApplyGameplaySettings([NewValue](FGameplaySettingsData& S) {
				S.MouseSensivity = NewValue;
			});
		}
	}
}

void UGameplaySettingsWidget::HandleToggleOnSprintChanged(bool bEnabled) {
	if (ToggleToSprintCheckBox) {
		ToggleToSprintCheckBox->SetIsChecked(bEnabled);

		FGameplaySettingsData NewSettings = {};

		NewSettings.bToggleToSprint = ToggleToSprintCheckBox->IsChecked();

		if (ULocalGlobalSettings* GlobalSettings = GetLocalGlobalSettings()) {
			GlobalSettings->SetAndApplyGameplaySettings([bEnabled](FGameplaySettingsData& S) {
				S.bToggleToSprint = bEnabled;
			});
		}
	}
}

void UGameplaySettingsWidget::HandleFieldOfViewChanged(float NewValue) {
	if (FieldOfViewSlider && FieldOfViewValue) {
		FieldOfViewSlider->SetValue(NewValue);
		FieldOfViewValue->SetText(FText::AsNumber(NewValue));

		FGameplaySettingsData NewSettings = {};

		NewSettings.FieldOfView = FieldOfViewSlider->GetValue();

		if (ULocalGlobalSettings* GlobalSettings = GetLocalGlobalSettings()) {
			GlobalSettings->OnGameplaySettingsChanged.Broadcast(NewSettings);
		}
	}
}

void UGameplaySettingsWidget::SaveCurrentSettings() {
	if (ULocalGlobalSettings* GlobalSettings = GetLocalGlobalSettings()) {
		GlobalSettings->SaveAllSettings();
	}
}

void UGameplaySettingsWidget::NativeConstruct() {
	Super::NativeConstruct();

	if (MouseSensivityValue) {
		MouseSensivityValue->SetText(FText::AsNumber(1.f));
	}
	
	if (FieldOfViewValue) {
		FieldOfViewValue->SetText(FText::AsNumber(75.f));
	}

	if (MouseSensivitySlider) {
		MouseSensivitySlider->SetValue(1.f);
		MouseSensivitySlider->OnValueChanged.AddDynamic(this, &UGameplaySettingsWidget::HandleSensivityChanged);
	}

	if (ToggleToSprintCheckBox) {
		ToggleToSprintCheckBox->SetIsChecked(true);
		ToggleToSprintCheckBox->OnCheckStateChanged.AddDynamic(this, &UGameplaySettingsWidget::HandleToggleOnSprintChanged);
	}

	if (FieldOfViewSlider) {
		FieldOfViewSlider->SetValue(75.f);
		FieldOfViewSlider->OnValueChanged.AddDynamic(this, &UGameplaySettingsWidget::HandleFieldOfViewChanged);
	}

	if (SaveButton) {
		SaveButton->OnClicked.AddDynamic(this, &UGameplaySettingsWidget::SaveCurrentSettings);
	}
}