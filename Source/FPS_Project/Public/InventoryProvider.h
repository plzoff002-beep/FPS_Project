// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InventoryProvider.generated.h"

class UItemDefinition;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInventoryProvider : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FPS_PROJECT_API IInventoryProvider
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
		UItemDefinition* TakeItem();
};
