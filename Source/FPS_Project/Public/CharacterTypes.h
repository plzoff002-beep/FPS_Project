#pragma once

#include "CharacterTypes.generated.h"

//enum class ECharacterEventType : uint8 {
//	None,
//	HealthEvent,
//	StaminaEvent,
//	AmmoEvent
//};

USTRUCT(BlueprintType)
struct FCharacterState {
	GENERATED_BODY()

	//VALUES

	UPROPERTY()
		float Velocity = 0.f;

	//STATES
	
	UPROPERTY(BlueprintReadOnly)
		bool bInAir = false;

	UPROPERTY(BlueprintReadOnly)
		bool bAiming = false;

	UPROPERTY(BlueprintReadOnly)
		bool bCloseToWall = false;

	//MOVEMENT

	UPROPERTY(BlueprintReadOnly)
		bool bWalking = false;

	UPROPERTY(BlueprintReadOnly)
		bool bSprinting = false;

	UPROPERTY(BlueprintReadOnly)
		bool bCrouching = false;
};

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

//struct FCharacterEventPayload {
//	ECharacterEventType EventType = ECharacterEventType::None;
//	float Value = -1.f;
//	float MaxValue = -1.f;
//
//	FCharacterEventPayload() {}
//	FCharacterEventPayload(ECharacterEventType NewType, float NewValue, float NewMaxValue)
//		: EventType(NewType), Value(NewValue), MaxValue(NewMaxValue) {}
//};