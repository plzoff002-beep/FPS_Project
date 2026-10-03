// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "UITypes.h"
#include "UIDelegates.h"
#include "FPSButton.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API UFPSButton : public UButton
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "UI")
		FText Text;

	FOnButtonClicked OnButtonClicked;
	FOnQualityLevelChanged OnQualityLevelChanged;

	void BindDelegate();

	void SetButtonIndex(int32 NewIndex);
	void SetButtonText(FText NewText);

private:
	int32 ButtonIndex = -1;

	UPROPERTY(EditAnywhere, Category = "UI")
		EButtonType ButtonType = EButtonType::None;

	UPROPERTY(EditAnywhere, Category = "UI")
		 UTextBlock* ButtonText = nullptr;

	UFUNCTION()
		void HandleInternalClick();
};
