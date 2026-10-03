// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "Weapon_BREN2.generated.h"

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API AWeapon_BREN2 : public AWeaponBase
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

protected:
	virtual void Tick(float DeltaTime) override;
};
