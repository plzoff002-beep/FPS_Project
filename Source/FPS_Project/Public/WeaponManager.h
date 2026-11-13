#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponDelegates.h"
#include "WeaponTypes.h"
#include "ItemDelegates.h"
#include "WeaponManager.generated.h"

class AWeaponBase;
class UWeaponData;
class UItemFragment_Weapon;
class UFPSStateComponent;

enum class EAttachmentType : uint8;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPS_PROJECT_API UWeaponManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWeaponManager();

	FORCEINLINE EWeaponState GetCurrentState() const { return CurrentState; }

	FORCEINLINE bool IsReloading() const { return CurrentState == EWeaponState::Reloading; }
	FORCEINLINE bool IsHoldingWeapon() const { return CurrentWeapon ? true : false; }

	FORCEINLINE int32 GetCurrentWeaponIndex() const { return CurrentWeaponIndex; }

	FOnShoot OnShoot;
	FOnItemLoaded OnWeaponLoaded;
	FOnWeaponChanged OnWeaponChanged;
	FOnWeaponMontageStarted OnWeaponMontageStarted;
	FOnAmmoChanged OnAmmoChanged;

	FOnItemAction OnItemAction;

	UPROPERTY(EditAnywhere)
		TArray<TSubclassOf<AWeaponBase>> WeaponClasses;

	void TryShoot() const;
	void TryReload() const;
	void TryInsertAmmo();
	void TryMoveWeaponStock();
	void TryToggleLaserAttachments();
	void TrySpawnCasing();
	void TryPlayEventSound(EWeaponAnimEvent EventType);

	const USkeletalMeshComponent* GetWeaponMesh() const;
	const UStaticMeshComponent* GetAttachmentMesh(EAttachmentType TargetType) const;

	void SwapWeapon(int32 Index);

	void LoadWeaponAsync(TSoftClassPtr<AWeaponBase> WeaponClass, const int32 SlotIndex, USceneComponent* AttachTo);
	void HandleWeaponLoaded(UClass* ClassToSpawn, const int32 SlotIndex, USceneComponent* AttachTo);

	void HandleMontageEvent(UWeaponData* Data, EWeaponMontage MontageType);
		
	void HandleAmmoChanged(const int32 CurrentAmmoValue, const int32 TotalAmmoValue) const;
	void HandleMontageFinished(EWeaponAnimStage StageType);

	void ImmediateUnEquip();

	void FinishHolster();
	void FinishEquip();

	AWeaponBase* GetCurrentWeapon() const;
	int32 GetCurrentAmmo() const;
	int32 GetTotalAmmo() const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY() TObjectPtr<UFPSStateComponent> StateComp = nullptr;
	UPROPERTY() TObjectPtr<AWeaponBase> CurrentWeapon = nullptr;

	EWeaponState CurrentState = EWeaponState::Ready;

	TArray<TObjectPtr<AWeaponBase>> WeaponsArr;
	
	int32 CurrentWeaponIndex = INDEX_NONE;
	int32 PendingWeaponIndex = INDEX_NONE;

	AWeaponBase* SpawnWeapon(UClass* WeaponClass, USceneComponent* AttachTo);

	void HandleSetReloadState(UWeaponData*, EWeaponMontage);
	void HandleSetReadyState();

	void HandleAmmoInserted();
	void HandleReloadFinished();
	void HandleTryChargeWeapon();
	void HandleChangeSpread();

	void ActivateWeapon(AWeaponBase* Weapon);
	void DeactivateWeapon();

	void BindWeaponDelegates();
	void UnbindWeaponDelegates();
};