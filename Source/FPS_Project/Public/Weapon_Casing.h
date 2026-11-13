// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon_Casing.generated.h"

class UProjectileMovementComponent;

UCLASS()
class FPS_PROJECT_API AWeapon_Casing : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon_Casing();
	
	virtual void Tick(float DeltaTime) override;

	bool IsActive() const { return bActive; }

	void ActivateCasing();
	void DisableCasing();

protected:
	virtual void BeginPlay() override;

	void Initialize();

	UPROPERTY(EditAnywhere)
	float SpeedValue = -1.f;

	UPROPERTY(EditAnywhere)
	float GravityValue = -1.f;

	UPROPERTY(EditAnywhere)
	float BounceValue = -1.f;

	UPROPERTY(EditAnywhere)
	float MaxLifeTime = -1.f;

	float CurrentLifeTime = 0.f;

	bool bActive = false;

	UPROPERTY(EditAnywhere)
		TObjectPtr<UStaticMeshComponent> CasingMesh = nullptr;

	UPROPERTY(VisibleAnywhere)
		UProjectileMovementComponent* ProjectileMovement = nullptr;
};
