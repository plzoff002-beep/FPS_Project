// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundMix.h"
#include "SoundTypes.h"
#include "SystemData.generated.h"

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API USystemData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Sound")
		TObjectPtr<USoundMix> GlobalSettingsMix = nullptr;
		
	UPROPERTY(EditAnywhere, Category = "Sound")
		TMap<USoundClass*, ESoundClassType> SoundMap;
};
