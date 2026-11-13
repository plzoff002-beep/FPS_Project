#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "UITypes.h"
#include "UIDelegates.h"
#include "UIData.h"
#include "LocalUIManager.generated.h"

class UUserWidget;
class UInventoryMenuWidget;
class UMenuWidget;
class UFPSGlobalSettingsSave;

UCLASS()
class FPS_PROJECT_API ULocalUIManager : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	FOnGameplayActionRequest OnGameplayActionRequest;

	UUserWidget* PushWidget(const FWidgetData* WidgetData, UObject* Context);
	void PopWidget();

	void SetUIData(UUIData* NewData);

	bool HasWidget(EUIType TargetType) const;
	bool FindAndPopWidget(EUIType TargetType);

	void FindAndPushWidget(EUIType TargetType, UObject* Context, TFunction<void(UUserWidget* Widget)> OnReady = nullptr);
	void ReplaceWidget(EUIType TargetType);

	template<typename T>
	T* GetWidget() const {
		for (const FRunTimeWidgetData& WidgetData : WidgetStack) {
			if (WidgetData.WidgetClass && WidgetData.WidgetClass.Get()->IsA(T::StaticClass())) {
				return Cast<T>(WidgetData.WidgetClass.Get());
			}
		}

		return nullptr;
	}

protected:
	UPROPERTY(EditAnywhere)
		TObjectPtr<UUIData> Data = nullptr;

	UPROPERTY() TObjectPtr<UFPSGlobalSettingsSave> CurrentSettings;

	TWeakObjectPtr<UObject> CurrentActiveContext = nullptr;

	TArray<FRunTimeWidgetData> WidgetStack;
	TMap<EUIType, int32> PendingActivations;

	bool bGameStarted = false;
	uint32 NextRequestId = 0;

	void LoadWidgetAsync(const FWidgetData& WidgetData, UObject* Context, bool bActivateOnLoad, TFunction<void(UUserWidget* Widget)> OnReady = nullptr);
	void HandleWidgetLoaded(const FWidgetData& WidgetData, UObject* Context, bool bActivateOnLoad, uint32 RequestId, TFunction<void(UUserWidget* Widget)> OnReady);

	void ClearPendingActivations();

	void ApplyInputMode();
	void ClearStack();

	void UnifiedActionHandler(EButtonType ButtonType, UObject*);

	void PreloadWidgets();

	void RemoveModalWidgets();
	void RefreshLayoutVisibility();

	UUserWidget* GetWidgetRaw(EUIType TargetType);
};
