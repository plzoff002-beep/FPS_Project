// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponDelegates.h"
#include "WeaponTypes.h"
#include "AttachmentTypes.h"
#include "WeaponBase.generated.h"

class UWeaponData;
class UWeaponAttachmentComponent;

struct FWeaponIKData;

USTRUCT()
struct FWeaponStats {
	GENERATED_BODY()

	float Damage = 0.f;
	float FireRate = 0.f;
	float Recoil = 0.f;
	float Spread = 0.f;
	float AdsSpread = 0.f;
	float SuppressPenalty = 0.f;
	float Range = 0.f;

	int32 ClipSize = 0;
};

UCLASS()
class FPS_PROJECT_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetActorHiddenInGame(bool bNewHidden) override;

	// Sets default values for this actor's properties
	AWeaponBase();

	FORCEINLINE UWeaponAttachmentComponent* GetWeaponAttachmentComponent() const { return AttachmentComponent; }

	FWeaponIKData GetWeaponIKData() const;

	FOnWeaponAction OnWeaponAction;
	FOnAmmoChanged OnAmmoChanged;

	virtual void Reload();

	void AddAmmo(int32 Value);

	void ReloadFinished();

	bool IsClipFull() const;
	bool IsChargeEveryShoot() const;

	USkeletalMeshComponent* WeaponMesh = nullptr;
	UStaticMeshComponent* MagazineMesh = nullptr;

	FORCEINLINE bool HasReserveAmmo() const { return TotalAmmo > 0; }
	FORCEINLINE bool HasCurrentAmmo() const { return Ammo > 0; }
	FORCEINLINE bool GetIsNeedCharge() const { return bNeedCharge; }

	FORCEINLINE int32 GetCurrentAmmo() const { return Ammo; }
	FORCEINLINE int32 GetTotalAmmo() const { return TotalAmmo; }

	FORCEINLINE UWeaponAttachmentComponent* GetAttachmentComponent() const { return AttachmentComponent; }

	UPROPERTY(EditAnywhere, Category = "Weapon | Attachment System")
		UWeaponAttachmentComponent* AttachmentComponent = nullptr;

	UPROPERTY(EditAnywhere)
		UWeaponData* Data = nullptr;

	UPROPERTY(VisibleAnywhere)
		float FireCooldown = 0.f;

	UFUNCTION(BlueprintCallable)
		bool CanShoot() const;

	UFUNCTION(BlueprintCallable)
		virtual void Shoot();

	void SetIsNeedCharge(bool bValue);
	void HandleChangeSpread();

	void PlayEventSound(EWeaponAnimEvent EventType) const;

	void SpawnCasing() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//void HandleHit(const FHitResult& Hit);
	//void HandleSuppress(const FVector& Origin, const FVector& Direction);

	// const float MaxSuppressDistance = 500.f;

	bool bNeedCharge = false;
	bool bWasWeaponEmpty = false;

	int32 Ammo = 0;
	int32 TotalAmmo = 0;

private:
	void Initialize();

	void AddModifiersStats();

	void InitStats();

	FWeaponStats WeaponStats;
};
