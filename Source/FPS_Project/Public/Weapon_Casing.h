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
	// Sets default values for this actor's properties
	AWeapon_Casing();

	UPROPERTY(EditAnywhere)
		float SpeedValue = -1.f;

	UPROPERTY(EditAnywhere)
		float GravityValue = -1.f;

	UPROPERTY(EditAnywhere)
		float BounceValue = -1.f;

	UPROPERTY(EditAnywhere)
		float LifeTime = -1.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
		TObjectPtr<UStaticMeshComponent> CasingMesh = nullptr;

	UPROPERTY(VisibleAnywhere)
		UProjectileMovementComponent* ProjectileMovement = nullptr;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void Initialize();
};
