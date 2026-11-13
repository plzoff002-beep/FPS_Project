// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WeaponIKTypes.h"
#include "CharacterTypes.h"
#include "ViewModelTypes.h"
#include "CharacterTypes.h"
#include "StateTypes.h"
#include "FPSAnimInstance.generated.h"

class AFPSCharacter;
class UFPSStateComponent;

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API UFPSAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

	void SetOwner(AActor* NewOwner);
	void SetStateComp(UFPSStateComponent* NewStateComp);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "IK")
		FTransform CurrentLeftHandIKTransform;

	UPROPERTY(BlueprintReadOnly, Category = "IK")
		FWeaponIKData IKData = {};

	UPROPERTY(BlueprintReadOnly, Category = "IK")
		float CurrentIKAlpha;

	UPROPERTY(EditAnywhere, Category = "IK Settings")
		float IKAccel = 15.f;

	UPROPERTY(EditAnywhere, Category = "IK Settings")
		float LHandIKAccel = 15.f;

	UPROPERTY(BlueprintReadOnly, Category = "Character State")
		FStateList StateList = {};

	UPROPERTY(BlueprintReadOnly, Category = "Character State")
		FViewModelState ViewModelState = {};

	TWeakObjectPtr<AActor> Owner = nullptr;
	TWeakObjectPtr<UFPSStateComponent> StateComp = nullptr;

	void UpdateLeftHandIK(float DeltaSeconds);

	void TryGetCharacterState();
	void TryGetViewModelState();
	void TryGetWeaponIKData();
};
