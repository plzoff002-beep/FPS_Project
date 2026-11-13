#pragma once

#include "CoreMinimal.h"
#include "Kismet/KismetMathLibrary.h"
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

USTRUCT(BlueprintType)
struct FFloatSpring {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float AimPenaltyValue = 1.f;

	UPROPERTY(EditAnywhere)
	float VelocityClampMin = -10.f;

	UPROPERTY(EditAnywhere)
	float VelocityClampMax = 10.f;

	UPROPERTY(BlueprintReadOnly)
	float CurrentVelocity = 0.f;

	float TargetVelocity = 0.f;

	FFloatSpringState SpringState;

	UPROPERTY(EditAnywhere)
	float Stiffness = 1.f;

	UPROPERTY(EditAnywhere)
	float CriticalDampingFactor = 1.f;

	UPROPERTY(EditAnywhere)
	float Mass = 1.f;

};

USTRUCT(BlueprintType)
struct FJumpSpring {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float JumpSensivity = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FFloatSpring VelocityZ;
};

USTRUCT(BlueprintType)
struct FSwaySpring {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FFloatSpring InputX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FFloatSpring InputY;
};

USTRUCT(BlueprintType)
struct FMovementSpring {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float MovementSensivity = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FFloatSpring MovementX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FFloatSpring MovementY;
};