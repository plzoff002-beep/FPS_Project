// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UITypes.h"
#include "UIDelegates.h"
#include "FPSButton.h"
#include "BaseButtonInterface.h"
#include "MenuWidget.generated.h"

class UButton;

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API UMenuWidget : public UUserWidget, public IBaseButtonInterface
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	virtual FOnButtonClicked& GetButtonDelegate() override { return OnButtonClicked; }

	FOnButtonClicked OnButtonClicked;

private:
	void UnifiedButtonHandler(EButtonType ButtonType, UObject*);

	UPROPERTY(meta = (BindWidget))
		UFPSButton* PlayButton = nullptr;

	UPROPERTY(meta = (BindWidget))
		UFPSButton* OptionsButton = nullptr;

	UPROPERTY(meta = (BindWidget))
		UFPSButton* QuitButton = nullptr;
};
