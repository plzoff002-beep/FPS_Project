// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "Weapon_Mossberg590.generated.h"

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API AWeapon_Mossberg590 : public AWeaponBase
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void Shoot() override;
};
