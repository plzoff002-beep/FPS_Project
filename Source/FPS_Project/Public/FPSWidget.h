// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "FPSWidget.generated.h"

class UProgressBar;

struct FCharacterEventPayload;

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API UFPSWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeomtry, float InDeltaTime) override;

	void InitHUD(const float HealthAmount);

	void SetHealth(const float HealthValue);

private:
	UPROPERTY(meta = (BindWidget))
		UTextBlock* CurrentAmmo = nullptr;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* TotalAmmo = nullptr;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* Health = nullptr;

	UPROPERTY(meta = (BindWidget))
		UCanvasPanel* Crosshair = nullptr;

	UPROPERTY(meta = (BindWidget))
		UProgressBar* StaminaBar = nullptr;

	void HandleAmmoChanged(const int32 CurrentAmmoValue, const int32 TotalAmmoValue);

	void HandleStaminaChanged(const float NewValue);
};
