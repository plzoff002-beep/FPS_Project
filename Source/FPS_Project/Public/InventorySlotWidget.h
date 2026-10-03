// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemTypes.h"
#include "InventoryDelegates.h"
#include "UISlotIndexReceiver.h"
#include "InventorySlotWidget.generated.h"

class UImage;
class UTextBlock;
class UButton;

enum class EButtonType : uint8;

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API UInventorySlotWidget : public UUserWidget, public IUISlotIndexReceiver
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	virtual int32 GetSlotIndex() override { return SlotIndex;}

	void UpdateSlot(const FInventorySlot& SlotInfo);

	void SetSlotIndex(int32 NewIndex);

	TMap<EButtonType, FText>* GetContextSlotActionMap() const;
	void SetContextSlotActionMap(TMap<EButtonType, FText>* NewMap);

	//FOnButtonClicked OnButtonClicked;

protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UPROPERTY(meta = (BindWidget))
		UImage* ItemIcon = nullptr;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* QuantityText = nullptr;

	UPROPERTY(meta = (BindWidget))
		UButton* SlotButton = nullptr;

private:
	/*UFUNCTION()
		void HandleButtonClicked();*/

	TMap<EButtonType, FText>* ContextActionMap = nullptr;

	int32 SlotIndex = INDEX_NONE;
};
