// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UITypes.h"
#include "ButtonExampleWidget.generated.h"

class UButton;
class UTextBlock;

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API UButtonExampleWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	FOnButtonClicked OnButtonClicked;

	void SetButtonType(EButtonType NewType);
	void SetButtonText(FText NewText);

protected:
#if WITH_EDITOR
	virtual void SynchronizeProperties() override;
#endif

	virtual void NativeConstruct() override;

private:
	UPROPERTY(EditAnywhere)
		EButtonType ButtonType = EButtonType::None;

	UPROPERTY(EditAnywhere, Category = "UI")
		FText ButtonText;

	UPROPERTY(meta = (BindWidget))
		UButton* Button = nullptr;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* TextBlock = nullptr;

	UFUNCTION()
		void UnifiedButtonHandler();
};
