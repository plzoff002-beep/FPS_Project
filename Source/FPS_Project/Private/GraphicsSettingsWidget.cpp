// Fill out your copyright notice in the Description page of Project Settings.


#include "GraphicsSettingsWidget.h"
#include "GameFramework/GameUserSettings.h"
#include "GraphicsOptionRow.h"
#include "FPSButton.h"
#include "Components/CheckBox.h"
#include "FPSGlobalSettingsSave.h"
#include "LocalGlobalSettings.h"
#include "SettingsTypes.h"

void UGraphicsSettingsWidget::NativeConstruct() {
	Super::NativeConstruct();

	if (OptionRowShadows) {
		OptionRowShadows->OnQualityLevelChanged.AddUObject(this, &UGraphicsSettingsWidget::ApplyShadowQuality);
	}

	if (OptionRowTextures) {
		OptionRowTextures->OnQualityLevelChanged.AddUObject(this, &UGraphicsSettingsWidget::ApplyTextureQuality);
	}

	if (SaveButton) {
		SaveButton->OnClicked.AddDynamic(this, &UGraphicsSettingsWidget::SaveSettings);
	}

	if (VSyncCheckBox) {
		VSyncCheckBox->OnCheckStateChanged.AddDynamic(this, &UGraphicsSettingsWidget::SetVSync);
	}
}

ULocalGlobalSettings* UGraphicsSettingsWidget::GetLocalGlobalSettings() {
	APlayerController* PC = Cast<APlayerController>(GetOwningPlayer());
	if (!PC) return nullptr;

	return PC->GetLocalPlayer()->GetSubsystem<ULocalGlobalSettings>();
}

void UGraphicsSettingsWidget::ApplyTextureQuality(int32 NewLevel) {
	ULocalGlobalSettings* GlobalSettings = GetLocalGlobalSettings();
	if (!GlobalSettings) return;

	GlobalSettings->SetAndApplyGraphicsSettings([NewLevel](FGraphicsSettingsData& S) {
		S.TexturesQualityLevel = NewLevel;
	});
}

void UGraphicsSettingsWidget::ApplyShadowQuality(int32 NewLevel) {
	ULocalGlobalSettings* GlobalSettings = GetLocalGlobalSettings();
	if (!GlobalSettings) return;

	GlobalSettings->SetAndApplyGraphicsSettings([NewLevel](FGraphicsSettingsData& S) {
		S.ShadowsQualityLevel = NewLevel;
	});
}

void UGraphicsSettingsWidget::SetVSync(bool bEnable) {
	ULocalGlobalSettings* GlobalSettings = GetLocalGlobalSettings();
	if (!GlobalSettings) return;

	GlobalSettings->SetAndApplyGraphicsSettings([bEnable](FGraphicsSettingsData& S) {
		S.bVSyncEnabled = bEnable;
		});
}

void UGraphicsSettingsWidget::SaveSettings() {
	if (ULocalGlobalSettings* GlobalSettings = GetLocalGlobalSettings()) {
		GlobalSettings->SaveAllSettings();
	}
}