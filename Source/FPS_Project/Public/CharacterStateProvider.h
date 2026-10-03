// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CharacterTypes.h"
#include "ViewModelTypes.h"
#include "CharacterStateProvider.generated.h"

// This class does not need to be modified.
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

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
		FCharacterState GetCharacterState() const;

	UFUNCTION(BlueprintNativeEvent)
		FViewModelState GetViewModelState() const;
};
