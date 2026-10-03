// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterTypes.h"
#include "CharacterStatsDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API UCharacterStatsDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Character Stats")
		TMap<ECharacterStatType, float> CharacterStatMap;

	float GetStat(ECharacterStatType StatType, float DefaultStat = 0.f) const {
		for (auto& Type : CharacterStatMap) {
			if (Type.Key == StatType) return Type.Value;
		}

		return DefaultStat;
	}
};
