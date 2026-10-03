// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FPSAnimInstance.h"
#include "Weapon_Casing.h"
#include "WeaponTypes.h"
#include "WeaponData.generated.h"

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API UWeaponData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
		float Damage = -1.f;

	UPROPERTY(EditAnywhere)
		float Range = -1.f;

	UPROPERTY(EditAnywhere)
		float FireRate = -1.f;

	UPROPERTY(EditAnywhere)
		float Spread = -1.f;

	UPROPERTY(EditAnywhere)
		float AdsSpread = -1.f;

	UPROPERTY(EditAnywhere)
		float Recoil = -1.f;

	UPROPERTY(EditAnywhere)
		bool bHasStock = false;

	UPROPERTY(EditAnywhere)
		int32 ShotsAmount = -1;

	UPROPERTY(EditAnywhere)
		int32 ClipSize = 0;

	UPROPERTY(EditAnywhere)
		int32 TotalAmmo = 0;

	UPROPERTY(EditAnywhere)
		int32 SuppressPenalty = 0;

	UPROPERTY(EditAnywhere)
		EWeaponReloadType ReloadType;

	UPROPERTY(EditAnywhere)
		bool bChargeEveryShoot = false;

	UPROPERTY(EditANywhere)
		FName Socket = TEXT("WeaponSocket");

	UPROPERTY(EditAnywhere)
		FName SocketMagazine = TEXT("SocketMagazine");

	UPROPERTY(EditAnywhere)
		FName SocketCasing = TEXT("SocketCasing");

	UPROPERTY(EditAnywhere)
		TSubclassOf<UFPSAnimInstance> ViewModelAnimBP = nullptr;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UAnimInstance> WeaponAnimBP = nullptr;

	UPROPERTY(EditAnywhere)
		TObjectPtr<USkeletalMesh> WeaponMesh = nullptr;

	UPROPERTY(EditAnywhere)
		TObjectPtr<UStaticMesh> MagazineMesh = nullptr;

	UPROPERTY(EditAnywhere)
		TSubclassOf<AWeapon_Casing> CasingMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Montage Animations Arms")
		TMap<EWeaponMontage, TObjectPtr<UAnimMontage>> MontageAnimations_Arms;

	UPROPERTY(EditDefaultsOnly, Category = "Montage Animations Weapon")
		TMap<EWeaponMontage, TObjectPtr<UAnimMontage>> MontageAnimations_Weapon;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		TMap<EWeaponAnimEvent, TObjectPtr<USoundBase>> AnimEventSounds;
};
