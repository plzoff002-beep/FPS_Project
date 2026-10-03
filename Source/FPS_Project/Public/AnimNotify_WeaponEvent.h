// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "WeaponTypes.h"
#include "AnimNotify_WeaponEvent.generated.h"

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API UAnimNotify_WeaponEvent : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Weapon Event")
		EWeaponAnimEvent EventType;
};
