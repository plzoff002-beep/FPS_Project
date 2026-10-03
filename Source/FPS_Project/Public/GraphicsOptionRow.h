// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UIDelegates.h"
#include "GraphicsOptionRow.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API UGraphicsOptionRow : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Settings")
		FText SettingName;

	FOnQualityLevelChanged OnQualityLevelChanged;

protected:
#if WITH_EDITOR
	virtual void SynchronizeProperties() override;
#endif

	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
		UTextBlock* TextLabel = nullptr;

	void UnifiedButtonHandler(int32 Index);
};
