#pragma once

#include "Blueprint/UserWidget.h"
#include "UITypes.generated.h"

UENUM(BlueprintType)
enum class EUILayerType : uint8 {
	None,
	Game,
	Menu,
	Modal
};

UENUM(BlueprintType)
enum class EButtonType : uint8
{
	None,
	Play,
	Options,
	ContextMenu,
	Quit,
	Back,
	Save,
	Drop,
	Delete,
	Discard,
	Gameplay,
	Graphics,
	Audio
};

UENUM(BlueprintType)
enum class EUIType : uint8 {
	None,
	Menu,
	Options,
	ContextMenu,
	Gameplay,
	Examination,
	Inventory
};

struct FRunTimeWidgetData {
	TObjectPtr<UUserWidget> WidgetClass = nullptr;
	EUIType UIType = EUIType::None;

	FRunTimeWidgetData(UUserWidget* Widget, EUIType Type) : 
		WidgetClass(Widget), UIType(Type) {}
};

USTRUCT(BlueprintType)
struct FWidgetData {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "UI")
		TSoftClassPtr<UUserWidget> WidgetClass = nullptr;

	UPROPERTY(EditAnywhere, Category = "UI")
		EUILayerType LayerType = EUILayerType::None;

	UPROPERTY(EditAnywhere, Category = "UI")
		EUIType UIType = EUIType::None;

	UPROPERTY(EditAnywhere, Category = "UI")
		bool bPreload = false;

	UPROPERTY(EditAnywhere, Category = "UI")
		bool bActivateOnLoaded = false;
};

USTRUCT(BlueprintType)
struct FGameplaySettings {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Gameplay Settings")
		float MouseSensivity = 1.f;

	UPROPERTY(EditAnywhere, Category = "Gameplay Settings")
		float FieldOfView = 70.f;

	UPROPERTY(EditAnywhere, Category = "Gameplay Settings")
		bool bToggleToSprint = true;
};