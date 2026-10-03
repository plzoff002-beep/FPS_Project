// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GraphicsSettingsWidget.generated.h"

class UFPSButton;
class UGraphicsOptionRow;
class UCheckBox;
class ULocalGlobalSettings;

enum class EButtonType : uint8;

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API UGraphicsSettingsWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

private:
	void ApplyShadowQuality(int32 NewLevel);
	void ApplyTextureQuality(int32 NewLevel);
	void SetVSync(bool bEnable);

	UFUNCTION()
		void SaveSettings();

	ULocalGlobalSettings* GetLocalGlobalSettings();

	UPROPERTY(meta = (BindWidget))
		UFPSButton* SaveButton = nullptr;

	UPROPERTY(meta = (BindWidget))
		UCheckBox* VSyncCheckBox = nullptr;

	UPROPERTY(meta = (BindWidget))
		UGraphicsOptionRow* OptionRowShadows = nullptr;

	UPROPERTY(meta = (BindWidget))
		UGraphicsOptionRow* OptionRowTextures = nullptr;
};
