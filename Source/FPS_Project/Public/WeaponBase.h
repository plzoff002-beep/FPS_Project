#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponDelegates.h"
#include "WeaponTypes.h"
#include "AttachmentTypes.h"
#include "WeaponBase.generated.h"

class UWeaponData;
class UWeaponAttachmentComponent;
class AWeapon_Casing;

struct FWeaponIKData;

UCLASS()
class FPS_PROJECT_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetActorHiddenInGame(bool bNewHidden) override;

	AWeaponBase();

	FWeaponIKData GetWeaponIKData() const;

	FOnWeaponAction OnWeaponAction;
	FOnAmmoChanged OnAmmoChanged;

	virtual bool Reload();
	virtual bool Shoot();

	void AddAmmo(int32 Value);

	void ReloadFinished();

	bool IsClipFull() const;
	bool IsChargeEveryShoot() const;

	bool CanShoot() const;
	
	FORCEINLINE bool HasData() { return Data ? true : false; }

	const float GetRecoil() const;

	UStaticMeshComponent* GetAttachmentMesh(EAttachmentType TargetType) const;
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh.Get(); }

	FORCEINLINE bool HasReserveAmmo() const { return TotalAmmo > 0; }
	FORCEINLINE bool HasCurrentAmmo() const { return Ammo > 0; }
	FORCEINLINE bool GetIsNeedCharge() const { return bNeedCharge; }

	FORCEINLINE int32 GetCurrentAmmo() const { return Ammo; }
	FORCEINLINE int32 GetTotalAmmo() const { return TotalAmmo; }
	
	FORCEINLINE UWeaponData* GetWeaponData() { return Data.Get(); }
	FORCEINLINE UWeaponAttachmentComponent* GetAttachmentComponent() const { return AttachmentComponent.Get(); }

	void SetIsNeedCharge(bool bValue);
	void HandleChangeSpread();

	void HandlePlayEventSound(EWeaponAnimEvent EventType) const;
	void HandleSpawnCasing() const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY() TObjectPtr<USkeletalMeshComponent> WeaponMesh = nullptr;
	UPROPERTY() TObjectPtr<UStaticMeshComponent> MagazineMesh = nullptr;
	UPROPERTY() TObjectPtr<UWeaponAttachmentComponent> AttachmentComponent = nullptr;

	UPROPERTY(EditAnywhere)
		TObjectPtr<UWeaponData> Data = nullptr;

	TArray<TObjectPtr<AWeapon_Casing>> CasingPool;

	FWeaponStats WeaponStats;

	bool bNeedCharge = false;
	bool bWasWeaponEmpty = false;

	int32 Ammo = 0;
	int32 TotalAmmo = 0;

	float FireCooldown = 0.f;

	void Initialize();
	void InitializeStats();
	void SpawnCasingPool();

	void AddModifiersStats();
};
