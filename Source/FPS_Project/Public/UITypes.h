#pragma once

#include "Blueprint/UserWidget.h"
#include "UITypes.generated.h"

UENUM(BlueprintType)
enum class EUILayerType : uint8 {
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

USTRUCT(BlueprintType)
struct FWidgetData {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "UI")
		TSubclassOf<UUserWidget> WidgetClass = nullptr;

	UPROPERTY(EditAnywhere, Category = "UI")
		EUILayerType LayerType = EUILayerType::Game;
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