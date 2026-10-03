#pragma once

class UWeaponData;
enum class EWeaponMontage : uint8;
enum class EWeaponAnimStage : uint8;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnWeaponChanged, UWeaponData*);

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnWeaponMontageStarted, UWeaponData*, EWeaponMontage);

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnWeaponAction, UWeaponData*, EWeaponMontage);

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnAmmoChanged, const int32, const int32);

DECLARE_MULTICAST_DELEGATE(FOnHolsterFinished);
DECLARE_MULTICAST_DELEGATE(FOnEquipFinished);

DECLARE_MULTICAST_DELEGATE(FOnInsertAmmoFinished);

DECLARE_MULTICAST_DELEGATE(FOnReloadStartFinished);
DECLARE_MULTICAST_DELEGATE(FOnReloadFinished);
DECLARE_MULTICAST_DELEGATE(FOnReloadEndFinished);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnMontageFinished, EWeaponAnimStage);