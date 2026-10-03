// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemFragment.generated.h"

class AWeaponBase;

/**
 * 
 */
UCLASS(Abstract, BlueprintType, EditInlineNew, DefaultToInstanced)
class FPS_PROJECT_API UItemFragment : public UObject
{
	GENERATED_BODY()
	
};

UCLASS()
class FPS_PROJECT_API UItemFragment_Weapon : public UItemFragment {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Weapon")
		TSubclassOf<AWeaponBase> WeaponClass = nullptr;
};
