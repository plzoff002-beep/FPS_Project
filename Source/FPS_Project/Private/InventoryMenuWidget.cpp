// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryMenuWidget.h"
#include "InventoryComponent.h"
#include "Components/UniformGridPanel.h"
#include "InventorySlotWidget.h"
#include "Components/UniformGridSlot.h"

void UInventoryMenuWidget::UnifiedButtonTriggerHandler(EButtonType ButtonType, UObject* Context) {
	if (OnButtonClicked.IsBound()) {
		OnButtonClicked.Broadcast(ButtonType, Context);
	}
}

void UInventoryMenuWidget::BindToInventory(UInventoryComponent* InventoryComp) {
	LinkedInventory = InventoryComp;

	if (LinkedInventory) {
		LinkedInventory->OnInventoryChanged.AddUObject(
			this,
			&UInventoryMenuWidget::RefreshInventory
		);

		RefreshInventory(0);
	}
}

void UInventoryMenuWidget::RefreshInventory(int32) {
	if (!InventoryGrid || !SlotWidgetClass || !LinkedInventory) return;

	const TArray<FInventorySlot>& Slots = LinkedInventory->GetSlots();

	const int32 ExistingWidgetsCount = InventoryGrid->GetChildrenCount();

	for (int32 i = 0; i < Slots.Num(); ++i) {
		UInventorySlotWidget* SlotWidget = nullptr;

		if (i < ExistingWidgetsCount) {
			SlotWidget = Cast<UInventorySlotWidget>(InventoryGrid->GetChildAt(i));
		}
		else {
			SlotWidget = CreateWidget<UInventorySlotWidget>(this, SlotWidgetClass);

			if (SlotWidget) {
				int32 Row = i / ColumnsCount;
				int32 Column = i % ColumnsCount;

				UUniformGridSlot* GridSlot = InventoryGrid->AddChildToUniformGrid(SlotWidget, Row, Column);

				if (GridSlot) {
					GridSlot->SetHorizontalAlignment(HAlign_Fill);
					GridSlot->SetVerticalAlignment(VAlign_Fill);
				}
			}
		}

		if (SlotWidget) {
			SlotWidget->SetVisibility(ESlateVisibility::Visible);
			SlotWidget->UpdateSlot(Slots[i]);
			SlotWidget->SetSlotIndex(i);
			SlotWidget->SetContextSlotActionMap(Slots[i].ActionMap);
		}
	}

	for (int32 i = Slots.Num(); i < ExistingWidgetsCount; ++i) {
		if (UWidget* ExtraSlot = InventoryGrid->GetChildAt(i)) {
			ExtraSlot->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}