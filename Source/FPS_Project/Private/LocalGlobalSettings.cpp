// Fill out your copyright notice in the Description page of Project Settings.


#include "LocalGlobalSettings.h"
#include "Kismet/GameplayStatics.h"
#include "FPSGlobalSettingsSave.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "SystemData.h"

const FString ULocalGlobalSettings::GlobalSettingsSlot = TEXT("GlobalSettingsSave");

void ULocalGlobalSettings::SetAndApplyAudioSettings(TFunctionRef<void(FAudioSettingsData&)> AudioMutator) {
	if (!CurrentSettings) return;

	AudioMutator(CurrentSettings->Audio);
	
	ApplyAudioSettings();
}

void ULocalGlobalSettings::SetAndApplyGraphicsSettings(TFunctionRef<void(FGraphicsSettingsData&)> GraphicsMutator) {
	if (!CurrentSettings) return;
	
	GraphicsMutator(CurrentSettings->Graphics);
	
	ApplyGraphicsSettings();
}

void ULocalGlobalSettings::SetAndApplyGameplaySettings(TFunctionRef<void(FGameplaySettingsData&)> GameplayMutator) {
	if (!CurrentSettings) return;

	GameplayMutator(CurrentSettings->Gameplay);
	
	OnGameplaySettingsChanged.Broadcast(CurrentSettings->Gameplay);
}

void ULocalGlobalSettings::SetSystemData(TObjectPtr<USystemData> NewData) {
	if (!NewData) return;

	SystemData = NewData;
}

void ULocalGlobalSettings::ApplyGraphicsSettings() {
	if (UGameUserSettings* Settings = GEngine->GetGameUserSettings()) {
		Settings->SetShadowQuality(CurrentSettings->Graphics.ShadowsQualityLevel);
		Settings->SetTextureQuality(CurrentSettings->Graphics.TexturesQualityLevel);
		Settings->SetVSyncEnabled(CurrentSettings->Graphics.bVSyncEnabled);

		Settings->ApplySettings(false);
	}
}

void ULocalGlobalSettings::ApplyAudioSettings() {
	if (!SystemData || !SystemData->GlobalSettingsMix || SystemData->SoundMap.IsEmpty()) return;

	for (auto& Sound : SystemData->SoundMap) {
		if (!Sound.Key) continue;

		float Volume = 1.f;
		if (float* Found = CurrentSettings->Audio.SoundVolumes.Find(Sound.Value)) {
			Volume = *Found;
		}

		UGameplayStatics::SetSoundMixClassOverride(
			GetWorld(),
			SystemData->GlobalSettingsMix,
			Sound.Key,
			Volume,
			0.f,
			1.f,
			true
		);
	}

	UGameplayStatics::PushSoundMixModifier(GetWorld(), SystemData->GlobalSettingsMix);
}

void ULocalGlobalSettings::ApplyGlobalEngineSettings() {
	if (!CurrentSettings) return;

	if (UGameUserSettings* GSettings = GEngine->GetGameUserSettings()) {
		GSettings->SetShadowQuality(CurrentSettings->Graphics.ShadowsQualityLevel);
		GSettings->SetTextureQuality(CurrentSettings->Graphics.TexturesQualityLevel);
	}

	OnGameplaySettingsChanged.Broadcast(CurrentSettings->Gameplay);
	ApplyGraphicsSettings();
	ApplyAudioSettings();
}

void ULocalGlobalSettings::LoadAndApplyAllSettings() {
	if (UGameplayStatics::DoesSaveGameExist(GlobalSettingsSlot, 0)) {
		CurrentSettings = Cast<UFPSGlobalSettingsSave>(UGameplayStatics::LoadGameFromSlot(GlobalSettingsSlot, 0));
	}
	else {
		CurrentSettings = Cast<UFPSGlobalSettingsSave>(UGameplayStatics::CreateSaveGameObject(UFPSGlobalSettingsSave::StaticClass()));
	}
}

void ULocalGlobalSettings::SaveAllSettings() {
	if (CurrentSettings) {
		UGameplayStatics::SaveGameToSlot(CurrentSettings, GlobalSettingsSlot, 0);
	}
}

void ULocalGlobalSettings::Initialize(FSubsystemCollectionBase& Collection) {
	Super::Initialize(Collection);

	LoadAndApplyAllSettings();
	ApplyGlobalEngineSettings();
}