// Fill out your copyright notice in the Description page of Project Settings.


#include "AudioSettingsWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Slider.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundMix.h"
#include "FPSButton.h"
#include "LocalGlobalSettings.h"
#include "Components/TextBlock.h"
#include "SettingsTypes.h"
#include "SystemData.h"
#include "SoundTypes.h"

ULocalGlobalSettings* UAudioSettingsWidget::GetGlobalSettings() {
	if (APlayerController* PC = Cast<APlayerController>(GetOwningPlayer())) {
		return PC->GetLocalPlayer()->GetSubsystem<ULocalGlobalSettings>();
	}

	return nullptr;
}

void UAudioSettingsWidget::SaveSettings() {
	if (ULocalGlobalSettings* GlobalSettings = GetGlobalSettings()) {
		GlobalSettings->SaveAllSettings();
	}
}

void UAudioSettingsWidget::HandleMasterVolumeChanged(float NewVolume) {
	if (MasterVolumeValue && MasterVolumeSlider) {
		MasterVolumeValue->SetText(FText::AsNumber(MasterVolumeSlider->GetValue()));
	}

	if (ULocalGlobalSettings* GlobalSettings = GetGlobalSettings()) {
		GlobalSettings->SetAndApplyAudioSettings([NewVolume](FAudioSettingsData& S) {
			*S.SoundVolumes.Find(ESoundClassType::Master) = NewVolume;
		});
	}
}

void UAudioSettingsWidget::NativeConstruct() {
	Super::NativeConstruct();

	if (MasterVolumeSlider) {
		MasterVolumeSlider->OnValueChanged.AddDynamic(this, &UAudioSettingsWidget::HandleMasterVolumeChanged);
	}

	if (SaveButton) {
		SaveButton->OnClicked.AddDynamic(this, &UAudioSettingsWidget::SaveSettings);
	}
}