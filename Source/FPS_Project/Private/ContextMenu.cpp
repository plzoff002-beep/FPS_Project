// Fill out your copyright notice in the Description page of Project Settings.


#include "ContextMenu.h"
#include "ButtonExampleWidget.h"
#include "InventorySlotWidget.h"
#include "Components/VerticalBox.h"
#include "Blueprint/WidgetTree.h"

bool UContextMenu::NativeOnHandleBackAction() {
    DeactivateWidget();

    return true;
}

void UContextMenu::UnifiedButtonHandler(EButtonType ButtonType, UObject*) {
    if (OnButtonClicked.IsBound()) {
        OnButtonClicked.Broadcast(ButtonType, this);
    }
}

void UContextMenu::SetContextItem(UObject* NewContext) {
    if (!NewContext) return;

    if (UInventorySlotWidget* SlotWidget = Cast<UInventorySlotWidget>(NewContext)) {
        if (!SlotWidget->GetContextSlotActionMap()) return;

        StoredContext = NewContext;
        SlotIndex = SlotWidget->GetSlotIndex();

        InitializeMenu(SlotWidget->GetContextSlotActionMap());
    }
}

void UContextMenu::InitializeMenu(const TMap<EButtonType, FText>* ActionMap) {
    if (!ActionButtonClass || !ActionList || !ActionMap || ActionMap->IsEmpty()) return;

    for (UWidget* Widget : ActionList->GetAllChildren()) {
        if (UButtonExampleWidget* Button = Cast<UButtonExampleWidget>(Widget)) {
            Button->OnButtonClicked.RemoveAll(this);
        }
    }

    ActionList->ClearChildren();

    for (const auto& Action : *ActionMap) {
        UButtonExampleWidget* NewButton = WidgetTree->ConstructWidget<UButtonExampleWidget>(ActionButtonClass);

        if (NewButton) {
            NewButton->SetButtonType(Action.Key);
            NewButton->SetButtonText(Action.Value);

            NewButton->OnButtonClicked.AddUObject(this, &UContextMenu::UnifiedButtonHandler);

            ActionList->AddChild(NewButton);
        }
    }
}