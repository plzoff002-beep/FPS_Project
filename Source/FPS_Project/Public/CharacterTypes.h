#pragma once

#include "CharacterTypes.generated.h"

USTRUCT(BlueprintType)
struct FPlayerGameplayConfig {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Player Gameplay Config")
		float MouseSensivity = 1.f;

	UPROPERTY(EditAnywhere, Category = "Player Gameplay Config")
		float FieldOfView = 75.f;

	UPROPERTY(EditAnywhere, Category = "Player Gameplay Config")
		bool bToggleToSprint = true;
};

UENUM(BlueprintType)
enum class ECharacterStatType : uint8 {
	None,
	MaxHealth,
	MaxStamina,
	StaminaDrainRate,
	StaminaRegenRate,
	SpeedAccel,
	WalkSpeed,
	SprintSpeed
};