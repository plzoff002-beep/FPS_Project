#include "LocalUIManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "InventoryMenuWidget.h"
#include "MenuWidget.h"
#include "FPSWidget.h"
#include "UITypes.h"
#include "SettingsMenuWidget.h"
#include "GameplaySettingsWidget.h"
#include "ExaminationWidget.h"
#include "ContextMenu.h"
#include "BaseButtonInterface.h"
#include "UIContextReceiver.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

UUserWidget* ULocalUIManager::PushWidget(const FWidgetData* WidgetData, UObject* Context) {
	if (!WidgetData || !WidgetData->WidgetClass.IsValid() || HasWidget(WidgetData->UIType)) return nullptr;

	UUserWidget* NewWidget = CreateWidget<UUserWidget>(GetWorld(), WidgetData->WidgetClass.Get());

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

		FRunTimeWidgetData RunTimeWidgetData(NewWidget, WidgetData->UIType);
		WidgetStack.Add(RunTimeWidgetData);

		if (WidgetData->LayerType == EUILayerType::Game) {
			NewWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		}

		ClearPendingActivations();

		ApplyInputMode();
		RefreshLayoutVisibility();
	}

	return NewWidget;
}

UUserWidget* ULocalUIManager::GetWidgetRaw(EUIType TargetType) {
	for (FRunTimeWidgetData& WidgetData : WidgetStack) {
		if (WidgetData.WidgetClass && WidgetData.UIType == TargetType) return WidgetData.WidgetClass.Get();
	}

	return nullptr;
}

void ULocalUIManager::FindAndPushWidget(EUIType TargetType, UObject* Context, TFunction<void(UUserWidget* Widget)> OnReady) {
	if (Data) {
		if (FWidgetData* WidgetToPush = Data->FindWidget(TargetType)) {
			LoadWidgetAsync(*WidgetToPush, Context, true, OnReady);
		}
	}
}

bool ULocalUIManager::HasWidget(EUIType TargetType) const {
	for (FRunTimeWidgetData WidgetData : WidgetStack) {
		if (WidgetData.WidgetClass && WidgetData.UIType == TargetType) return true;
	}

	return false;
}

bool ULocalUIManager::FindAndPopWidget(EUIType TargetType) {
	for (int32 i = 0; i < WidgetStack.Num(); ++i) {
		if (WidgetStack[i].WidgetClass && WidgetStack[i].UIType == TargetType) {
			WidgetStack[i].WidgetClass.Get()->RemoveFromParent();
			WidgetStack.RemoveAt(i);

			ApplyInputMode();
			RefreshLayoutVisibility();

			return true;
		}
	}

	return false;
}

void ULocalUIManager::ClearPendingActivations() {
	PendingActivations.Empty();
	NextRequestId = 0;
}

void ULocalUIManager::HandleWidgetLoaded(const FWidgetData& WidgetData, UObject* Context, bool bActivateOnLoad, uint32 RequestId, TFunction<void(UUserWidget* Widget)> OnReady) {
	if (!WidgetData.WidgetClass.IsValid()) {
		UE_LOG(LogTemp, Error, TEXT("LocalUIManager: HandleWidgetLoaded is failed Loading"));
		return;
	}
	else if (!Context && Data) {
		UE_LOG(LogTemp, Warning, TEXT("LocalUIManager: HandleWidgetLoaded - Context has nullptr in Widget Data: %s"),
			*UEnum::GetValueAsString(WidgetData.UIType)
		);
	}
	else if (!Context && !Data) {
		UE_LOG(LogTemp, Error, TEXT("LocalUIManager: HandleWidgetLoaded - Can't find Widget Index while Data linking is Failed"));
	}

	if (bActivateOnLoad) {
		const auto* LatestId = PendingActivations.Find(WidgetData.UIType);
		if (!LatestId || *LatestId != RequestId) return;

		PendingActivations.Remove(WidgetData.UIType);

		UUserWidget* Widget = PushWidget(&WidgetData, Context);
		if (Widget && OnReady) {
			OnReady(Widget);
		}
	}
}

void ULocalUIManager::LoadWidgetAsync(const FWidgetData& WidgetData, UObject* Context, bool bActivateOnLoad, TFunction<void(UUserWidget* Widget)> OnReady) {
	if (WidgetData.WidgetClass.IsNull()) {
		UE_LOG(LogTemp, Error, TEXT("LocalUIManager: LoadWidgetAsync - SoftClass is Null"));
		return;
	}

	uint32 RequestId = 0;

	if (bActivateOnLoad) {
		RequestId = ++NextRequestId;
		PendingActivations.Add(WidgetData.UIType, RequestId);
	}

	if (WidgetData.WidgetClass.IsValid()) {
		HandleWidgetLoaded(WidgetData, Context, bActivateOnLoad, RequestId, OnReady);
		return;
	}

	TWeakObjectPtr<ULocalUIManager> WeakThis(this);
	TWeakObjectPtr<UObject> WeakObject(Context);

	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	Streamable.RequestAsyncLoad(
		WidgetData.WidgetClass.ToSoftObjectPath(),
		FStreamableDelegate::CreateLambda([WeakThis, WidgetData, WeakObject, OnReady, bActivateOnLoad, RequestId] {
			if (WeakThis.IsValid()) {
				WeakThis->HandleWidgetLoaded(WidgetData, WeakObject.Get(), bActivateOnLoad, RequestId, OnReady);
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("LocalUIManager: Manager linking is Failed"));
			}
		})
	);
}

void ULocalUIManager::PreloadWidgets() {
	if (!Data) return;

	TArray<FWidgetData>& DataArr = Data.Get()->WidgetClasses;

	for (const FWidgetData& WidgetData : DataArr) {
		if (WidgetData.WidgetClass.IsNull()) {
			UE_LOG(LogTemp, Warning, TEXT("LocalUIManager: PreloadWidgets - SoftClass is nullptr in Widget Data: %i"),
				*UEnum::GetValueAsString(WidgetData.UIType)
			);

			continue;
		} 

		if (WidgetData.bPreload) {
			bool bActivateOnLoaded = WidgetData.bActivateOnLoaded;
			LoadWidgetAsync(WidgetData, nullptr, bActivateOnLoaded);
		}
	}

	PushWidget(Data.Get()->FindWidget(EUIType::Menu), nullptr);
}

void ULocalUIManager::RefreshLayoutVisibility() {
	if (WidgetStack.IsEmpty()) return;

	bool bShouldHideGameHUD = false;

	if (WidgetStack.Num() > 1) {
		bShouldHideGameHUD = HasWidget(EUIType::Examination);
	}

	if (UFPSWidget* GameHUD = GetWidget<UFPSWidget>()) {
		GameHUD->SetVisibility(bShouldHideGameHUD ? ESlateVisibility::Collapsed : ESlateVisibility::HitTestInvisible);
	}
}

void ULocalUIManager::RemoveModalWidgets() {
	for (int32 i = WidgetStack.Num() - 1; i >= 0; --i) {
		if (WidgetStack[i].WidgetClass.Get()->IsA(UContextMenu::StaticClass())) {
			WidgetStack[i].WidgetClass.Get()->RemoveFromParent();
			WidgetStack.RemoveAt(i);
		}
	}
}

void ULocalUIManager::SetUIData(UUIData* NewData) {
	if (!NewData) return;

	Data = NewData;
	
	if (!bGameStarted) {
		PreloadWidgets();
		bGameStarted = true;
	}
}

void ULocalUIManager::ReplaceWidget(EUIType TargetType) {
	if (WidgetStack.IsEmpty()) return;

	PopWidget();
	FindAndPushWidget(TargetType, nullptr);
}

void ULocalUIManager::UnifiedActionHandler(EButtonType ButtonType, UObject*) {
	switch (ButtonType) {
	case EButtonType::Play:
		ReplaceWidget(EUIType::Gameplay);
		break;

	case EButtonType::Options:
		if (FWidgetData* WidgetData = Data->FindWidget(EUIType::Options)) {
			LoadWidgetAsync(*WidgetData, nullptr, true);
		}

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
		OnGameplayActionRequest.Broadcast(ButtonType, CurrentActiveContext.Get());
		break;

	case EButtonType::Drop:
		OnGameplayActionRequest.Broadcast(ButtonType, CurrentActiveContext.Get());
		break;

	case EButtonType::Delete:
		OnGameplayActionRequest.Broadcast(ButtonType, CurrentActiveContext.Get());
		break;

		case EButtonType::Discard:
		PopWidget();
		break;
	}
}

void ULocalUIManager::ClearStack() {
	if (WidgetStack.IsEmpty()) return;

	for (FRunTimeWidgetData& WidgetData : WidgetStack) {
		WidgetData.WidgetClass.Get()->RemoveFromParent();
	}

	WidgetStack.Empty();
}

void ULocalUIManager::PopWidget() {
	if (WidgetStack.Num() > 0) {
		FRunTimeWidgetData& TopData = WidgetStack.Last();
		TopData.WidgetClass.Get()->RemoveFromParent();

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

	for (const FRunTimeWidgetData& WidgetData : WidgetStack) {
		if (WidgetData.WidgetClass && WidgetData.WidgetClass.Get()->bIsFocusable) {
			bNeedWidget = true;
			TopFocusableWidget = WidgetData.WidgetClass.Get();
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