// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ViewModelTypes.h"
#include "CharacterStateProvider.generated.h"

UINTERFACE(MinimalAPI)
class UCharacterStateProvider : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FPS_PROJECT_API ICharacterStateProvider
{
	GENERATED_BODY()

public:
	virtual FViewModelState GetViewModelState() = 0;
};
