// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "Templates/Function.h"
#include "SettingsDelegates.h"
#include "LocalGlobalSettings.generated.h"

class USystemData;
class UFPSGlobalSettingsSave;

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API ULocalGlobalSettings : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	static const FString GlobalSettingsSlot;

	FOnGameplaySettingsChanged OnGameplaySettingsChanged;

	void SetAndApplyGameplaySettings(TFunctionRef<void(FGameplaySettingsData&)> GameplayMutator);
	void SetAndApplyAudioSettings(TFunctionRef<void(FAudioSettingsData&)> AudioMutator);
	void SetAndApplyGraphicsSettings(TFunctionRef<void(FGraphicsSettingsData&)> GraphicsMutator);

	void SaveAllSettings();
	void LoadAndApplyAllSettings();
	void ApplyGlobalEngineSettings();

	void SetSystemData(TObjectPtr<USystemData> NewData);

	FORCEINLINE TObjectPtr<USystemData> GetSystemData() { return SystemData; }

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

private:
	void ApplyAudioSettings();
	void ApplyGraphicsSettings();

	UFPSGlobalSettingsSave* CurrentSettings = nullptr;

	TObjectPtr<USystemData> SystemData = nullptr;
};
