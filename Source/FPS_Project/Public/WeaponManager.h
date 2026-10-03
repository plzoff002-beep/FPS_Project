// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponDelegates.h"
#include "WeaponTypes.h"
#include "WeaponManager.generated.h"

class AWeaponBase;
class UWeaponData;
class UItemFragment_Weapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPS_PROJECT_API UWeaponManager : public UActorComponent
{
	GENERATED_BODY()

public:	

	// Sets default values for this component's properties
	UWeaponManager();

	FORCEINLINE EWeaponState GetCurrentState() const { return CurrentState; }

	FORCEINLINE bool IsReloading() const { return CurrentState == EWeaponState::Reloading; }

	FORCEINLINE int32 GetCurrentWeaponIndex() const { return CurrentWeaponIndex; }

	UPROPERTY(EditAnywhere)
		TArray<TSubclassOf<AWeaponBase>> WeaponClasses;

	void TryShoot() const;
	void TryReload() const;
	void TryInsertAmmo();
	void TryMoveWeaponStock();

	void HandleSetReloadState(UWeaponData*, EWeaponMontage);
	void HandleSetReadyState();

	void HandleMontageFinished(EWeaponAnimStage StageType);

	void SwapWeapon(int32 Index, AActor* Actor);

	//void SpawnWeapons(USceneComponent* AttachTo);
	AWeaponBase* SpawnWeapon(USceneComponent* AttachTo, UItemFragment_Weapon* WeaponFragment);

	void HandleMontageEvent(UWeaponData* Data, EWeaponMontage MontageType);
		
	void HandleAmmoChanged(const int32 CurrentAmmoValue, const int32 TotalAmmoValue) const;

	void HandleAmmoInserted();
	void HandleReloadFinished();
	void HandleTryChargeWeapon();
	void HandleChangeSpread();

	void ImmediateUnEquip();

	void FinishHolster();
	void FinishEquip();

	//void HandleAnimEvent(EWeaponAnimEvent EventType);

	AWeaponBase* GetCurrentWeapon() const;
	int32 GetCurrentAmmo() const;
	int32 GetTotalAmmo() const;

	FOnWeaponChanged OnWeaponChanged;
	FOnWeaponMontageStarted OnWeaponMontageStarted;
	FOnAmmoChanged OnAmmoChanged;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
		AWeaponBase* CurrentWeapon = nullptr;

	UPROPERTY(VisibleAnywhere)
		TArray<AWeaponBase*> WeaponsArr;
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	int32 CurrentWeaponIndex = INDEX_NONE;
	int32 PendingWeaponIndex = INDEX_NONE;

	EWeaponState CurrentState = EWeaponState::Ready;

	void ActivateWeapon(AWeaponBase* Weapon);
	void DeactivateWeapon();

	void BindWeaponDelegates();
	void UnbindWeaponDelegates();
};