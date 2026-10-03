#pragma once

#include "AttachmentTypes.generated.h"

class UAttachmentData;

UENUM(BlueprintType)
enum class EAttachmentType : uint8 {
	None	UMETA(DisplayName = "None"),
	Sight	UMETA(DisplayName = "Sight"),
	Grip	UMETA(DisplayName = "Grip"),
	Laser	UMETA(DisplayName = "Laser"),
	Flashlight	UMETA(DisplayName = "Flashlight"),
	Stock	UMETA(DisplayName = "Stock")
};

UENUM(BlueprintType)
enum class EAttachmentState : uint8 {
	None,
	Folded,
	Extended
};

USTRUCT(BlueprintType)
struct FWeaponStatModifiers {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float AdditiveDamage = 0.f;

	UPROPERTY(EditAnywhere)
	float RecoilMultiplier = 1.f;

	UPROPERTY(EditAnywhere)
	float SpreadMultiplier = 1.f;

	UPROPERTY(EditAnywhere)
	float AdsSpreadMultiplier = 1.f;
};

USTRUCT(BlueprintType)
struct FAttachmentRunTimeData {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
		TObjectPtr<UAttachmentData> Data = nullptr;

	UPROPERTY(EditAnywhere)
		EAttachmentState State = EAttachmentState::None;
};