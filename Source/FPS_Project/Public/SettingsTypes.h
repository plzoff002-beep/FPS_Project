#pragma once

#include "SettingsDelegates.h"
#include "SoundTypes.h"

struct FGameplaySettingsData {
	float MouseSensivity = 1.f;
	float FieldOfView = 75.f;

	bool bToggleToSprint = true;
};

struct FGraphicsSettingsData {
	int32 ShadowsQualityLevel = 3;
	int32 TexturesQualityLevel = 3;
	
	bool bVSyncEnabled = true;
};

struct FAudioSettingsData {
	TMap<ESoundClassType, float> SoundVolumes;
};