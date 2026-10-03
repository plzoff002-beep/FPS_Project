#pragma once

#include "CoreMinimal.h"
#include "ViewModelTypes.generated.h"

USTRUCT(BlueprintType)
struct FViewModelState {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
		float Rotation = 0.f;

	UPROPERTY(BlueprintReadOnly)
		float FlightValue = 0.f;

	UPROPERTY(BlueprintReadOnly)
		float MovementX = 0.f;

	UPROPERTY(BlueprintReadOnly)
		float MovementY = 0.f;

	UPROPERTY(BlueprintReadOnly)
		float SwayInputX = 0.f;

	UPROPERTY(BlueprintReadOnly)
		float SwayInputY = 0.f;
};