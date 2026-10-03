// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UIDelegates.h"
#include "GameplaySettingsWidget.generated.h"

class USlider;
class UCheckBox;
class UTextBlock;
class UFPSButton;
class ULocalGlobalSettings;

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API UGameplaySettingsWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

private:
	ULocalGlobalSettings* GetLocalGlobalSettings();

	UFUNCTION()
		void SaveCurrentSettings();

	UFUNCTION()
		void HandleSensivityChanged(float NewValue);

	UFUNCTION()
		void HandleFieldOfViewChanged(float NewValue);

	UFUNCTION()
		void HandleToggleOnSprintChanged(bool bEnabled);

	UPROPERTY(meta = (BindWidget))
		USlider* MouseSensivitySlider = nullptr;

	UPROPERTY(meta = (BindWidget))
		USlider* FieldOfViewSlider = nullptr;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* FieldOfViewValue = nullptr;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* MouseSensivityValue = nullptr;

	UPROPERTY(meta = (BindWidget))
		UCheckBox* ToggleToSprintCheckBox = nullptr;

	UPROPERTY(meta = (BindWidget))
		UFPSButton* SaveButton = nullptr;
};