// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "ItemDefinition.h"
#include "Components/Button.h"
#include "UITypes.h"
#include "LocalUIManager.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetLayoutLibrary.h"

TMap<EButtonType, FText>* UInventorySlotWidget::GetContextSlotActionMap() const {
	if (ContextActionMap->IsEmpty() || !ContextActionMap) {
		return nullptr;
	}

	return ContextActionMap;
}

FReply UInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) {
	if (!ContextActionMap) return FReply::Unhandled();

	if (APlayerController* PC = Cast<APlayerController>(GetOwningPlayer())) {
		if (ULocalUIManager* UIManager = PC->GetLocalPlayer()->GetSubsystem<ULocalUIManager>()) {
			if (UUserWidget* ContextMenu = Cast<UUserWidget>(UIManager->FindAndLoadWidget<UContextMenu>(this))) {
				//FVector2D MousePos = InMouseEvent.GetScreenSpacePosition();
				FVector2D LocalMousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());

				//USlateBlueprintLibrary::ScreenToWidgetLocal(this, InGeometry, MousePos, LocalMousePos);

				//ContextMenu->SetPositionInViewport(LocalMousePos, true);

				if (UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(ContextMenu)) {
					CanvasSlot->SetPosition(LocalMousePos);
				}

				//ContextMenu->SetRenderTranslation(LocalMousePos);

				return FReply::Handled();
			}
		}
	}

		return FReply::Unhandled();
}

void UInventorySlotWidget::SetContextSlotActionMap(TMap<EButtonType, FText>* NewMap) {
	ContextActionMap = NewMap;
}

void UInventorySlotWidget::SetSlotIndex(int32 NewIndex) {
	SlotIndex = NewIndex;
}

//void UInventorySlotWidget::HandleButtonClicked() {
//	if (OnButtonClicked.IsBound()) {
//		OnButtonClicked.Broadcast(EButtonType::ContextMenu, this);
//	}
//}

void UInventorySlotWidget::UpdateSlot(const FInventorySlot& SlotInfo) {
	if (SlotInfo.IsEmpty()) {
		ItemIcon->SetVisibility(ESlateVisibility::Collapsed);
		QuantityText->SetVisibility(ESlateVisibility::Collapsed);

		return;
	}

	ItemIcon->SetVisibility(ESlateVisibility::HitTestInvisible);

	TSoftObjectPtr<UTexture2D> IconPtr = SlotInfo.ItemDef->Icon;

	UTexture2D* Texture = IconPtr.Get();

	if (Texture) {
		ItemIcon->SetBrushFromTexture(Texture, false);
	}
	else {
		Texture = SlotInfo.ItemDef->Icon.LoadSynchronous();

		if (Texture) {
			ItemIcon->SetBrushFromTexture(Texture, false);
		}
	}

	if (SlotInfo.Quantity > 1) {
		QuantityText->SetVisibility(ESlateVisibility::HitTestInvisible);
		QuantityText->SetText(FText::AsNumber(SlotInfo.Quantity));
	}
	else {
		QuantityText->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UInventorySlotWidget::NativeConstruct() {
	Super::NativeConstruct();

	/*if (SlotButton) {
		SlotButton->OnClicked.AddDynamic(
			this,
			&UInventorySlotWidget::HandleButtonClicked
		);
	}*/
}