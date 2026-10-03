// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemTypes.h"
#include "ItemFragment.h"
#include "ItemDefinition.generated.h"

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API UItemDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Display")
		FName ItemName;

	UPROPERTY(EditAnywhere, Category = "Display")
		TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, Category = "Logic")
		EItemType ItemType = EItemType::None;

	UPROPERTY(EditAnywhere, Category = "Logic")
		int32 MaxStackSize = 1;

	UPROPERTY(EditAnywhere, Category = "Logic")
		TSoftClassPtr<AActor> WorldActorClass = nullptr;

	UPROPERTY(EditAnywhere, Category = "Logic")
		TMap<EButtonType, FText> ActionMap;

	UPROPERTY(EditAnywhere, Instanced, Category = "Fragments")
		TArray<TObjectPtr<UItemFragment>> Fragments;

	UPROPERTY(EditAnywhere, Category = "Examine")
		FExamineItem ExamineData;

	template<typename T>
	T* FindFragment() const {
		for (auto& Fragment : Fragments) {
			if (T* TypedFragment = Cast<T>(Fragment)) return TypedFragment;
		}

		return nullptr;
	}
};
