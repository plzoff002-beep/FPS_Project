// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "UITypes.h"
#include "UIDelegates.h"
#include "BaseButtonInterface.h"
#include "UIContextReceiver.h"
#include "ContextMenu.generated.h"

class UVerticalBox;
class UButtonExampleWidget;

/**
 * 
 */
UCLASS(Abstract)
class FPS_PROJECT_API UContextMenu : public UCommonActivatableWidget, public IBaseButtonInterface, public IUIContextReceiver
{
	GENERATED_BODY()
	
public:
	FOnButtonClicked OnButtonClicked;

	virtual void SetContextItem(UObject* NewContext) override;
	virtual FOnButtonClicked& GetButtonDelegate() override { return OnButtonClicked; }

protected:
	virtual bool NativeOnHandleBackAction() override;

private:
	int32 SlotIndex = INDEX_NONE;

	UObject* StoredContext = nullptr;

	void UnifiedButtonHandler(EButtonType ButtonType, UObject*);

	void InitializeMenu(const TMap<EButtonType, FText>* ActionMap);

	UPROPERTY(meta = (BindWidget))
		UVerticalBox* ActionList = nullptr;

	UPROPERTY(EditAnywhere, Category = "UI")
		TSubclassOf<UButtonExampleWidget> ActionButtonClass = nullptr;
};
