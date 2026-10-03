// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SettingsTypes.h"
#include "SoundTypes.h"
#include "FPSGlobalSettingsSave.generated.h"

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API UFPSGlobalSettingsSave : public USaveGame
{
	GENERATED_BODY()
	
public:
	FGameplaySettingsData Gameplay;
	FGraphicsSettingsData Graphics;
	FAudioSettingsData Audio;
};
