// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AudioSettingsWidget.generated.h"

class USlider;
class USoundClass;
class USoundMix;
class UFPSButton;
class UTextBlock;
class ULocalGlobalSettings;

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API UAudioSettingsWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
		void SaveSettings();

	ULocalGlobalSettings* GetGlobalSettings();

	UFUNCTION()
		void HandleMasterVolumeChanged(float NewVolume);

	UPROPERTY(meta = (BindWidget))
		USlider* MasterVolumeSlider = nullptr;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* MasterVolumeValue = nullptr;

	UPROPERTY(meta = (BindWidget))
		UFPSButton* SaveButton = nullptr;
};
