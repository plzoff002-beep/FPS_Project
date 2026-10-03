// Fill out your copyright notice in the Description page of Project Settings.


#include "LocalUIManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "InventoryMenuWidget.h"
#include "MenuWidget.h"
#include "FPSWidget.h"
#include "UIData.h"
#include "UITypes.h"
#include "SettingsMenuWidget.h"
#include "GameplaySettingsWidget.h"
#include "BaseButtonInterface.h"
#include "UIContextReceiver.h"

UUserWidget* ULocalUIManager::PushWidget(FWidgetData* WidgetData, UObject* Context) {
	if (!WidgetData || !WidgetData->WidgetClass) return nullptr;

	UUserWidget* NewWidget = CreateWidget<UUserWidget>(GetWorld(), WidgetData->WidgetClass);

	CurrentActiveContext = Context;

	if (IBaseButtonInterface* ButtonInterface = Cast<IBaseButtonInterface>(NewWidget)) {
		ButtonInterface->GetButtonDelegate().AddUObject(
			this,
			&ULocalUIManager::UnifiedActionHandler
		);
	}

	if (IUIContextReceiver* ContextReceiver = Cast<IUIContextReceiver>(NewWidget)) {
		ContextReceiver->SetContextItem(Context);
	}

	if (WidgetData->LayerType == EUILayerType::Modal) {
		RemoveModalWidgets();
	}

	if (NewWidget) {
		int32 ZOrder = static_cast<int32>(WidgetData->LayerType);

		NewWidget->AddToPlayerScreen(ZOrder * 10);
		WidgetStack.Add(NewWidget);

		if (WidgetData->LayerType == EUILayerType::Game) {
			NewWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		}

		ApplyInputMode();
		RefreshLayoutVisibility();
	}

	return NewWidget;
}

void ULocalUIManager::RefreshLayoutVisibility() {
	if (WidgetStack.IsEmpty()) return;

	bool bShouldHideGameHUD = false;

	if (WidgetStack.Num() > 1) {
		bShouldHideGameHUD = HasWidget<UExaminationWidget>();
	}

	if (UFPSWidget* GameHUD = GetWidget<UFPSWidget>()) {
		GameHUD->SetVisibility(bShouldHideGameHUD ? ESlateVisibility::Collapsed : ESlateVisibility::HitTestInvisible);
	}
}

void ULocalUIManager::RemoveModalWidgets() {
	for (int32 i = WidgetStack.Num() - 1; i >= 0; --i) {
		if (WidgetStack[i]->IsA<UContextMenu>()) {
			WidgetStack[i]->RemoveFromParent();
			WidgetStack.RemoveAt(i);
		}
	}
}

void ULocalUIManager::SetUIData(UUIData* NewData) {
	if (!NewData) return;

	Data = NewData;
	
	if (!bGameStarted) {
		PushWidget(Data->FindWidget<UMenuWidget>(), nullptr);

		bGameStarted = true;
	}
}

void ULocalUIManager::ReplaceWidget(FWidgetData* WidgetData) {
	if (WidgetStack.IsEmpty() || !WidgetData) return;

	PopWidget();
	PushWidget(WidgetData, nullptr);
}

void ULocalUIManager::UnifiedActionHandler(EButtonType ButtonType, UObject*) {
	switch (ButtonType) {
	case EButtonType::Play:
		ReplaceWidget(Data->FindWidget<UFPSWidget>());
		break;

	case EButtonType::Options:
		PushWidget(Data->FindWidget<USettingsMenuWidget>(), nullptr);
		break;

	case EButtonType::Quit:
		{
			APlayerController* PC = Cast<APlayerController>(GetLocalPlayer()->GetPlayerController(GetWorld()));

			if (PC) {
				UKismetSystemLibrary::QuitGame(this, PC, EQuitPreference::Quit, false);
			}

			break;
		}

	case EButtonType::Back:
		PopWidget();
		break;

	case EButtonType::Save:
		OnGameplayActionRequest.Broadcast(ButtonType, CurrentActiveContext);
		break;

	case EButtonType::Drop:
		OnGameplayActionRequest.Broadcast(ButtonType, CurrentActiveContext);
		break;

	case EButtonType::Delete:
		OnGameplayActionRequest.Broadcast(ButtonType, CurrentActiveContext);
		break;

		case EButtonType::Discard:
		PopWidget();
		break;
	}
}

void ULocalUIManager::ClearStack() {
	if (WidgetStack.IsEmpty()) return;

	for (auto* Widget : WidgetStack) {
		Widget->RemoveFromParent();
	}

	WidgetStack.Empty();
}

void ULocalUIManager::PopWidget() {
	if (WidgetStack.Num() > 0) {
		UUserWidget* TopWidget = WidgetStack.Last();
		TopWidget->RemoveFromParent();

		WidgetStack.RemoveAt(WidgetStack.Num() - 1);

		ApplyInputMode();
		RefreshLayoutVisibility();
	}
}

void ULocalUIManager::ApplyInputMode() {
	APlayerController* PC = GetLocalPlayer()->GetPlayerController(GetWorld());
	if (!PC) return;

	bool bNeedWidget = false;
	UUserWidget* TopFocusableWidget = nullptr;

	for (UUserWidget* Widget : WidgetStack) {
		if (Widget->bIsFocusable) {
			bNeedWidget = true;
			TopFocusableWidget = Widget;
		}
	}

	if (bNeedWidget && TopFocusableWidget) {
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(TopFocusableWidget->TakeWidget());
		InputMode.SetHideCursorDuringCapture(true);

		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
		PC->SetIgnoreLookInput(true);
	}
	else {
		PC->SetInputMode(FInputModeGameOnly());
		PC->bShowMouseCursor = false;
		PC->ResetIgnoreLookInput();
	}
}

void ULocalUIManager::Initialize(FSubsystemCollectionBase& Collection) {
	Super::Initialize(Collection);

}