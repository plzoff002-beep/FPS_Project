// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "UITypes.h"
#include "UIDelegates.h"
#include "LocalUIManager.generated.h"

class UUserWidget;
class UInventoryMenuWidget;
class UMenuWidget;
class UUIData;
class UFPSGlobalSettingsSave;

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API ULocalUIManager : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	FOnGameplayActionRequest OnGameplayActionRequest;

	UUserWidget* PushWidget(FWidgetData* WidgetData, UObject* Context);
	void PopWidget();

	void SetUIData(UUIData* NewData);

	template<typename T>
	T* GetWidget() const {
		for (auto* Widget : WidgetStack) {
			if (T* NeedWidget = Cast<T>(Widget)) return NeedWidget;
		}

		return nullptr;
	}

	template<typename T>
	bool HasWidget() const {
		if (WidgetStack.IsEmpty()) return false;

		for (auto* Widget : WidgetStack) {
			if (Widget->IsA<T>()) return true;
		}

		return false;
	}

	template<typename T>
	TSubclassOf<UUserWidget> FindAndLoadWidget(UObject* Context) {
		if (!Data) return nullptr;

		if (FWidgetData* NewData = Data->FindWidget<T>()) {
			PushWidget(NewData, Context);

			return NewData->WidgetClass;
		}

		return nullptr;
	}

	template<typename T>
	UUserWidget* FindAndPushWidget(UObject* Context) {
		if (!Data) return nullptr;

		if (FWidgetData* NewData = Data->FindWidget<T>()) {
			return PushWidget(NewData, Context);
		}

		return nullptr;
	}

	template<typename T>
	bool FindAndPopWidget() {
		if (WidgetStack.IsEmpty()) return false;

		for (int32 i = 0; i < WidgetStack.Num(); ++i) {
			if (WidgetStack[i] && WidgetStack[i]->IsA<T>()) {
				WidgetStack[i]->RemoveFromParent();
				WidgetStack.RemoveAt(i);

				ApplyInputMode();
				RefreshLayoutVisibility();

				return true;
			}
		}

		return false;
	}

private:
	UPROPERTY(EditAnywhere)
		TObjectPtr<UUIData> Data = nullptr;

	UObject* CurrentActiveContext = nullptr;

	TArray<UUserWidget*> WidgetStack;

	bool bGameStarted = false;

	void ApplyInputMode();
	void ClearStack();
	void ReplaceWidget(FWidgetData* WidgetData);

	void UnifiedActionHandler(EButtonType ButtonType, UObject*);

	void RemoveModalWidgets();
	void RefreshLayoutVisibility();

	UPROPERTY()
		UFPSGlobalSettingsSave* CurrentSettings;
};
