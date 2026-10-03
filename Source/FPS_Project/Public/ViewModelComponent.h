// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "WeaponTypes.h"
#include "ViewModelTypes.h"
#include "WeaponDelegates.h"
#include "ViewModelComponent.generated.h"

class AFPSCharacter;
class UWeaponData;
class UWeaponManager;

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

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPS_PROJECT_API UViewModelComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UViewModelComponent();

	FORCEINLINE FViewModelState GetState() const { return State; }

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FJumpSpring WeaponRotationConfig;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FJumpSpring WeaponHeightConfig;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FSwaySpring WeaponSwayConfig;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FMovementSpring WeaponMovementConfig;

	UPROPERTY(BlueprintReadOnly)
		FVector2D MouseInputThisFrame = (0, 0);
		
	FOnMontageFinished OnMontageFinished;

	void HandleMontageFinished(EWeaponAnimStage StageType) const;

	void UpdateViewModelState();

	void HandleWeaponAnimEvent(EWeaponAnimEvent EventType) const;
	void HandleWeaponMontageEvent(UWeaponData* Data, EWeaponMontage MontageType);
	void HandleWeaponCasingEvent() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Logic")
	float ADSAccel = 5.f;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void OnWeaponChanged(UWeaponData* Data);

private:
	AFPSCharacter* Owner = nullptr;
	UWeaponManager* WeaponManager = nullptr;

	void UpdateADS(float DeltaTime);

	void UpdateSpring(float DeltaTime, FFloatSpring& Config);
	void UpdateJumpTarget(FJumpSpring& Config);
	void UpdateSwayTarget(FSwaySpring& Config);
	void UpdateMovementTarget(FMovementSpring& Config);

	void StopAnimImmediately();

	FViewModelState State = {};

	EWeaponMontage CurrentMontage = EWeaponMontage::None;
};
