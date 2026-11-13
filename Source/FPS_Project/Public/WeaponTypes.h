#pragma once

struct FWeaponStats {
	float Damage = 0.f;
	float FireRate = 0.f;
	float Recoil = 0.f;
	float Spread = 0.f;
	float AdsSpread = 0.f;
	float SuppressPenalty = 0.f;
	float Range = 0.f;

	int32 ClipSize = 0;
};

UENUM(BlueprintType)
enum class EWeaponState : uint8 {
	None,
	Ready,
	Holstering,
	Equipping,
	Reloading,
	Charging,
	Stocking
};

UENUM(BlueprintType)
enum class EWeaponAnimEvent : uint8 {
	Shot,
	Deploy,
	MagOut,
	MagIn,
	MagClatch,
	Reload_Start,
	Reload_End,
	ChargeBack,
	ChargeForward,
	AmmoInsert,
	Stock
};

UENUM(BlueprintType)
enum class EWeaponAnimStage : uint8 {
	ShootFinished,
	EquipFinished,
	HolsterFinished,
	ReloadStartFinished,
	ReloadFinished,
	ReloadEndFinished,
	ChargeFinished,
	InsertAmmoFinished,
	StockFinished
};

UENUM(BlueprintType)
enum class EWeaponMontage : uint8 {
	None,
	Shoot,
	StockIn,
	StockOut,
	Reload_Start,
	Reload,
	Reload_Finish,
	Charge,
	Equip,
	Holster,
	InsertAmmo
};

UENUM(BlueprintType)
enum class EWeaponReloadType : uint8 {
	Standart,
	Staged
};