// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UITypes.h"
#include "UIDelegates.h"
#include "FPSButton.h"
#include "BaseButtonInterface.h"
#include "SettingsMenuWidget.generated.h"

class UWidgetSwitcher;

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API USettingsMenuWidget : public UUserWidget, public IBaseButtonInterface
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	virtual FOnButtonClicked& GetButtonDelegate() override { return OnButtonClicked; }

	FOnButtonClicked OnButtonClicked;

private:
	UPROPERTY(meta = (BindWidget))
		UWidgetSwitcher* SettingsSwitcher = nullptr;

	UPROPERTY(meta = (BindWidget))
		UFPSButton* BackButton = nullptr;

	UPROPERTY(meta = (BindWidget))
		UFPSButton* GameplayButton = nullptr;
	
	UPROPERTY(meta = (BindWidget))
		UFPSButton* GraphicsButton = nullptr;
	
	UPROPERTY(meta = (BindWidget))
		UFPSButton* AudioButton = nullptr;

	void UnifiedButtonHandler(EButtonType ButtonType, UObject*);
};
