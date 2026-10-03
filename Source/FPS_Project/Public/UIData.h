// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MenuWidget.h"
#include "FPSWidget.h"
#include "InventoryMenuWidget.h"
#include "SettingsMenuWidget.h"
#include "UITypes.h"
#include "UIData.generated.h"

//class UMenuWidget;
//class UFPSWidget;
//class UInventoryMenuWidget;

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API UUIData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "UI")
		TArray<FWidgetData> WidgetClasses;

	template <typename T>
	FWidgetData* FindWidget() {
		for (FWidgetData& WC : WidgetClasses) {
			if (WC.WidgetClass && WC.WidgetClass->IsChildOf(T::StaticClass())) return &WC;
		}

		return nullptr;
	}
};
