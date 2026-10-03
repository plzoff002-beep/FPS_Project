// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UIContextReceiver.h"
#include "ExaminationWidget.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API UExaminationWidget : public UUserWidget, public IUIContextReceiver
{
	GENERATED_BODY()

public:
	virtual void SetContextItem(UObject* NewContext) override;

private:
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UTextBlock> ItemNameText = nullptr;

	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UTextBlock> DescriptionText = nullptr;
};
