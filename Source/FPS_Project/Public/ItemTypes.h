#pragma once

#include "UITypes.h"
#include <ItemTypes.generated.h>

class UItemDefinition;
class AWeaponBase;

UENUM(BlueprintType)
enum class EItemType : uint8 {
	None,
	Consumable,
	Ammo,
	Weapon,
	Attachment
};

USTRUCT(BlueprintType)
struct FInventorySlot {
	GENERATED_BODY()

	TMap<EButtonType, FText>* ActionMap = nullptr;

	UPROPERTY()
		UItemDefinition* ItemDef = nullptr;

	UPROPERTY()
		TWeakObjectPtr<AActor> SpawnedActor = nullptr;

	UPROPERTY()
		EItemType ItemType;

	UPROPERTY()
		int32 Quantity = 0;

		FORCEINLINE bool IsEmpty() const { return Quantity <= 0 || !ItemDef; }
};

USTRUCT(BlueprintType)
struct FExamineItem {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Examine Settigs")
		TSoftClassPtr<AActor> ExamineActorClass = nullptr;

	UPROPERTY(EditAnywhere, Category = "Examine Settings")
		FText Name;

	UPROPERTY(EditAnywhere, Category = "Examine Settings")
		FText Description;

	UPROPERTY(EditAnywhere, Category = "Examine Settings")
		float InitialCameraDistance = 150.f;

	UPROPERTY(EditAnywhere, Category = "Examine Settings")
		float MaxCameraDistance = 500.f;

	UPROPERTY(EditAnywhere, Category = "Examine Settings")
		float MinCameraDistance = 150.f;
};