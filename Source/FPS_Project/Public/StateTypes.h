#pragma once

#include "WeaponTypes.h"
#include "StateTypes.generated.h"

UENUM(BlueprintType)
enum class EMovementState : uint8 {
	Idle,
	Walking,
	Sprinting,
};

UENUM(BlueprintType)
enum class EStanceState : uint8 {
	Standing,
	Crouching
};

USTRUCT(BlueprintType)
struct FStateList {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
		EMovementState MovementState = EMovementState::Idle;

	UPROPERTY(BlueprintReadOnly)
		EWeaponState WeaponState = EWeaponState::None;

	UPROPERTY(BlueprintReadOnly)
		EStanceState StanceState = EStanceState::Standing;

	UPROPERTY(BlueprintReadOnly)
		bool bAimed = false;

	UPROPERTY(BlueprintReadOnly)
		bool bFalling = false;

	UPROPERTY(BlueprintReadOnly)
		bool bCloseToWall = false;

	bool operator==(const FStateList& Other) const {
		return (MovementState == Other.MovementState &&
			WeaponState == Other.WeaponState &&
			StanceState == Other.StanceState &&
			bAimed == Other.bAimed &&
			bFalling == Other.bFalling &&
			bCloseToWall == Other.bCloseToWall
		);
	}

	bool operator!=(const FStateList& Other) const {
		return !(*this == Other);
	}
};