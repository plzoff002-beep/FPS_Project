// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponManager.h"
#include "WeaponBase.h"
#include "WeaponData.h"
#include "FPSCharacter.h"
#include "WeaponAttachmentComponent.h"
#include "ItemFragment.h"
#include "CharacterTypes.h"

// Sets default values for this component's properties
UWeaponManager::UWeaponManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

int32 UWeaponManager::GetTotalAmmo() const {
	if (!CurrentWeapon->Data) return 0;

	return CurrentWeapon->GetTotalAmmo();
}

int32 UWeaponManager::GetCurrentAmmo() const {
	if (!CurrentWeapon) return 0;

	return CurrentWeapon->GetCurrentAmmo();
}

AWeaponBase* UWeaponManager::GetCurrentWeapon() const {
	if (!CurrentWeapon) return nullptr;

	return CurrentWeapon;
}

void UWeaponManager::TryMoveWeaponStock() {
	if (CurrentState != EWeaponState::Ready) return;

	if (CurrentWeapon->GetAttachmentComponent()->HasStockAttachment()) {
		CurrentState = EWeaponState::Stocking;

		if (CurrentWeapon->GetAttachmentComponent()->IsStockExtended()) {
			CurrentWeapon->GetAttachmentComponent()->ToggleStockState(EAttachmentState::Folded);

			OnWeaponMontageStarted.Broadcast(CurrentWeapon->Data, EWeaponMontage::StockIn);
		}
		else {
			CurrentWeapon->GetAttachmentComponent()->ToggleStockState(EAttachmentState::Extended);

			OnWeaponMontageStarted.Broadcast(CurrentWeapon->Data, EWeaponMontage::StockOut);
		}
	}
}

void UWeaponManager::HandleTryChargeWeapon() {
	if (CurrentState == EWeaponState::Reloading && CurrentWeapon->GetIsNeedCharge()) {
		CurrentWeapon->SetIsNeedCharge(false);

		CurrentState = EWeaponState::Charging;

		OnWeaponMontageStarted.Broadcast(CurrentWeapon->Data, EWeaponMontage::Charge);

		return;
	}
	else if (CurrentWeapon->IsChargeEveryShoot() && CurrentWeapon->HasCurrentAmmo() && CurrentWeapon->GetIsNeedCharge()) {
		CurrentWeapon->SetIsNeedCharge(false);

		CurrentState = EWeaponState::Charging;
		
		OnWeaponMontageStarted.Broadcast(CurrentWeapon->Data, EWeaponMontage::Charge);

		return;
	}

	HandleSetReadyState();
}

void UWeaponManager::ImmediateUnEquip() {
	if (!CurrentWeapon) return;

	DeactivateWeapon();
	
	PendingWeaponIndex = -1;
	CurrentWeaponIndex = -1;

	CurrentState = EWeaponState::Ready;

	OnWeaponChanged.Broadcast(nullptr);
	OnAmmoChanged.Broadcast(0, 0);

	CurrentWeapon = nullptr;
}

void UWeaponManager::HandleChangeSpread() {
	CurrentWeapon->HandleChangeSpread();

	CurrentState = EWeaponState::Ready;
}

void UWeaponManager::HandleSetReloadState(UWeaponData*, EWeaponMontage) {
	CurrentState = EWeaponState::Reloading;
}

void UWeaponManager::HandleSetReadyState() {
	CurrentState = EWeaponState::Ready;
}

void UWeaponManager::HandleReloadFinished() {
	CurrentWeapon->ReloadFinished();

	if (CurrentWeapon->GetIsNeedCharge()) {
		HandleTryChargeWeapon();

		return;
	}

	HandleSetReadyState();
}

void UWeaponManager::TryInsertAmmo() {
	if (CurrentWeapon->IsClipFull() || !CurrentWeapon->HasReserveAmmo()) {
		OnWeaponMontageStarted.Broadcast(CurrentWeapon->Data, EWeaponMontage::Reload_Finish);

		return;
	}
	else {
		OnWeaponMontageStarted.Broadcast(CurrentWeapon->Data, EWeaponMontage::InsertAmmo);
	}
}

void UWeaponManager::HandleAmmoInserted() {
	CurrentWeapon->AddAmmo(1);

	TryInsertAmmo();
}

void UWeaponManager::UnbindWeaponDelegates() {
	if (!CurrentWeapon) return;

	CurrentWeapon->OnWeaponAction.RemoveAll(this);
	CurrentWeapon->OnAmmoChanged.RemoveAll(this);
}

void UWeaponManager::BindWeaponDelegates() {
	if (!CurrentWeapon) return;

	CurrentWeapon->OnWeaponAction.AddUObject(
		this,
		&UWeaponManager::HandleMontageEvent
	);

	CurrentWeapon->OnAmmoChanged.AddUObject(
		this,
		&UWeaponManager::HandleAmmoChanged
	);
}

void UWeaponManager::DeactivateWeapon() {
	if (!CurrentWeapon) return;

	CurrentWeapon->SetActorHiddenInGame(true);

	UnbindWeaponDelegates();
}

void UWeaponManager::ActivateWeapon(AWeaponBase* Weapon) {
	CurrentWeapon = Weapon;
	CurrentWeapon->SetActorHiddenInGame(false);

	BindWeaponDelegates();

	OnWeaponChanged.Broadcast(CurrentWeapon->Data);
	OnAmmoChanged.Broadcast(GetCurrentAmmo(), GetTotalAmmo());
}

void UWeaponManager::FinishEquip() {
	if (CurrentState != EWeaponState::Equipping) return;

	PendingWeaponIndex = -1;

	BindWeaponDelegates();

	HandleSetReadyState();
}

void UWeaponManager::FinishHolster() {
	if (CurrentState != EWeaponState::Holstering) return;

	if (CurrentWeapon) {
		DeactivateWeapon();
	}

	CurrentWeaponIndex = PendingWeaponIndex;

	int32 WeaponToActivate = CurrentWeaponIndex;

	GetWorld()->GetTimerManager().SetTimerForNextTick([this, WeaponToActivate]()
		{
			if (UInventoryComponent* Inv = GetOwner()->FindComponentByClass<UInventoryComponent>()) {
				if (AWeaponBase* SpawnedActor = Cast<AWeaponBase>(Inv->GetSlot(CurrentWeaponIndex)->SpawnedActor)) {
					ActivateWeapon(SpawnedActor);

					CurrentState = EWeaponState::Equipping;

					OnWeaponMontageStarted.Broadcast(CurrentWeapon->Data, EWeaponMontage::Equip);
				}
			}
		}
	);
}

void UWeaponManager::HandleAmmoChanged(const int32 CurrentAmmoValue, const int32 TotalAmmoValue) const {
	OnAmmoChanged.Broadcast(CurrentAmmoValue, TotalAmmoValue);

	/*float CurCopy = CurrentAmmoValue;
	float TotCopy = TotalAmmoValue;

	FCharacterEventPayload NewEventPayload{ ECharacterEventType::AmmoEvent, CurCopy, TotCopy };

	OnCharacterEvent.Broadcast(NewEventPayload);*/
}

void UWeaponManager::HandleMontageFinished(EWeaponAnimStage StageType) {
	switch (StageType) {
		case EWeaponAnimStage::ShootFinished:
			if (CurrentState != EWeaponState::Reloading) {
				HandleTryChargeWeapon();
			}
			break;

		case EWeaponAnimStage::EquipFinished:
			FinishEquip();
			break;

		case EWeaponAnimStage::HolsterFinished:
			FinishHolster();
			break;

		case EWeaponAnimStage::InsertAmmoFinished:
			HandleAmmoInserted();
			TryInsertAmmo();
			break;

		case EWeaponAnimStage::ReloadStartFinished:
			TryInsertAmmo();
			break;

		case EWeaponAnimStage::ReloadFinished:
			HandleReloadFinished();
			break;

		case EWeaponAnimStage::ReloadEndFinished:
			HandleTryChargeWeapon();
			break;

		case EWeaponAnimStage::ChargeFinished:
			HandleSetReadyState();
			break;

		case EWeaponAnimStage::StockFinished:
			HandleChangeSpread();
			break;
	}
}

void UWeaponManager::HandleMontageEvent(UWeaponData* Data, EWeaponMontage MontageType) {
	if (MontageType == EWeaponMontage::Reload || MontageType == EWeaponMontage::Reload_Start) {
		CurrentState = EWeaponState::Reloading;
	}

	OnWeaponMontageStarted.Broadcast(Data, MontageType);
}

void UWeaponManager::SwapWeapon(int32 Index, AActor* Actor) {
	if (CurrentWeaponIndex == Index || !Actor || CurrentState != EWeaponState::Ready) return;

	if (AWeaponBase* WeaponActor = Cast<AWeaponBase>(Actor)) {
		PendingWeaponIndex = Index;

		if (!CurrentWeapon) {
			CurrentState = EWeaponState::Ready;

			ActivateWeapon(WeaponActor);

			OnWeaponMontageStarted.Broadcast(CurrentWeapon->Data, EWeaponMontage::Equip);

			return;
		}

		CurrentState = EWeaponState::Holstering;

		OnWeaponMontageStarted.Broadcast(CurrentWeapon->Data, EWeaponMontage::Holster);
	}
}

//void UWeaponManager::SpawnWeapons(USceneComponent* AttachTo) {
//	if (WeaponClasses.IsEmpty()) return;
//
//	AFPSCharacter* Owner = Cast<AFPSCharacter>(GetOwner());
//
//	FActorSpawnParameters Params;
//	Params.Owner = Owner;
//
//	for (const TSubclassOf<AWeaponBase>& WeaponClass : WeaponClasses) {
//		if (!WeaponClass) continue;
//
//		AWeaponBase* Weapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, FTransform(), Params);
//
//		if (Weapon && AttachTo) {
//			Weapon->AttachToComponent(AttachTo, FAttachmentTransformRules::SnapToTargetIncludingScale, Weapon->Data->Socket);
//
//			Weapon->SetActorHiddenInGame(true);
//
//			WeaponsArr.Add(Weapon);
//		}
//	}
//
//	SwapWeapon(0);
//}

AWeaponBase* UWeaponManager::SpawnWeapon(USceneComponent* AttachTo, UItemFragment_Weapon* WeaponFragment) {
	if (WeaponFragment) {
		FActorSpawnParameters Params;
		Params.Owner = GetOwner();

		AWeaponBase* SpawnedActor = GetWorld()->SpawnActor<AWeaponBase>(WeaponFragment->WeaponClass, FTransform(), Params);

		if (SpawnedActor) {
			SpawnedActor->AttachToComponent(AttachTo, FAttachmentTransformRules::SnapToTargetIncludingScale, SpawnedActor->Data->Socket);
			SpawnedActor->SetActorHiddenInGame(true);

			return SpawnedActor;
		}
	}

	return nullptr;
}

void UWeaponManager::TryReload() const {
	if (!CurrentWeapon || CurrentState != EWeaponState::Ready) {
		return;
	}

	CurrentWeapon->Reload();
}

void UWeaponManager::TryShoot() const {
	if (!CurrentWeapon || CurrentState != EWeaponState::Ready || CurrentWeapon->GetIsNeedCharge()) {
		return;
	}

	CurrentWeapon->Shoot();
}

// Called when the game starts
void UWeaponManager::BeginPlay()
{
	Super::BeginPlay();

	// ...

}


// Called every frame
void UWeaponManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
