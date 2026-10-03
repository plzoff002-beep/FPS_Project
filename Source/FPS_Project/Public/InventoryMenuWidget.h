// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UIDelegates.h"
#include "InventoryMenuWidget.generated.h"

class UUniformGridPanel;
class UInvewntoryComponent;
class UInventorySlotWidget;

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API UInventoryMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void BindToInventory(UInventoryComponent* InventoryComp);

	FOnButtonClicked OnButtonClicked;

protected:
	UPROPERTY(meta = (BindWidget))
		UUniformGridPanel* InventoryGrid = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory UI")
		TSubclassOf<UInventorySlotWidget> SlotWidgetClass = nullptr;

	void RefreshInventory(int32);

private:
	UPROPERTY()
		UInventoryComponent* LinkedInventory = nullptr;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
		int32 ColumnsCount = 5;

	void UnifiedButtonTriggerHandler(EButtonType ButtonType, UObject* Context);
};
