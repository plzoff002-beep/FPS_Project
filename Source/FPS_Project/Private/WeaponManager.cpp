#include "WeaponManager.h"
#include "WeaponBase.h"
#include "WeaponData.h"
#include "FPSCharacter.h"
#include "WeaponAttachmentComponent.h"
#include "FPSStateComponent.h"
#include "ItemTypes.h"
#include "AttachmentTypes.h"
#include "CharacterTypes.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

UWeaponManager::UWeaponManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

int32 UWeaponManager::GetTotalAmmo() const {
	if (!CurrentWeapon) {
		UE_LOG(LogTemp, Error, TEXT("WeaponManager: GetTotalAmmo - CurrentWeapon is nullptr"));
		return 0;
	}

	return CurrentWeapon.Get()->GetTotalAmmo();
}

int32 UWeaponManager::GetCurrentAmmo() const {
	if (!CurrentWeapon) {
		UE_LOG(LogTemp, Error, TEXT("WeaponManager: GetCurrentAmmo - CurrentWeapon is nullptr"));
		return 0;
	}

	return CurrentWeapon.Get()->GetCurrentAmmo();
}

AWeaponBase* UWeaponManager::GetCurrentWeapon() const {
	if (!CurrentWeapon) {
		UE_LOG(LogTemp, Error, TEXT("WeaponManager: GetCurrentWeapon - CurrentWeapon is nullptr"));
		return nullptr;
	}

	return CurrentWeapon.Get();
}

const USkeletalMeshComponent* UWeaponManager::GetWeaponMesh() const {
	if (!CurrentWeapon) {
		UE_LOG(LogTemp, Error, TEXT("WeaponManager: GetWeaponMesh - CurrentWeapon is nullptr"));
		return nullptr;
	}

	return CurrentWeapon.Get()->GetWeaponMesh();
}

const UStaticMeshComponent* UWeaponManager::GetAttachmentMesh(EAttachmentType TargetType) const {
	if (!CurrentWeapon || !CurrentWeapon.Get()->GetAttachmentComponent()) {
		UE_LOG(LogTemp, Warning, TEXT("WeaponManager: GetAttachmentMesh. CurrentWeapon: %s, AttachmentComponent: %s"),
			*GetNameSafe(CurrentWeapon.Get()),
			(CurrentWeapon && CurrentWeapon.Get()->GetAttachmentComponent()) ? TEXT("True") : TEXT("False")
		);

		return nullptr;
	}

	return CurrentWeapon.Get()->GetAttachmentMesh(TargetType);
}

void UWeaponManager::HandleWeaponLoaded(UClass* ClassToSpawn, const int32 SlotIndex, USceneComponent* AttachTo) {
	if (!ClassToSpawn || !AttachTo) {
		UE_LOG(LogTemp, Error, TEXT("WeaponManager: OnWeaponLoaded failed. Class: %s, AttachTo: %s"), 
			*GetNameSafe(ClassToSpawn), 
			*GetNameSafe(AttachTo)
		);

		return;
	}

	if (AActor* SpawnedActor = Cast<AActor>(SpawnWeapon(ClassToSpawn, AttachTo))) {
		FLoadedItemData NewLoadedData{ SlotIndex, SpawnedActor};

		OnWeaponLoaded.Broadcast(NewLoadedData);
	}
}

void UWeaponManager::LoadWeaponAsync(TSoftClassPtr<AWeaponBase> WeaponClass, const int32 SlotIndex, USceneComponent* AttachTo) {
	if (WeaponClass.IsNull() || !AttachTo) {
		UE_LOG(LogTemp, Error, TEXT("WeaponManager: Async Loading failed. WeaponClass: %s, AttachTo: %s"),
			WeaponClass.IsNull() ? TEXT("False") : TEXT("True"),
			*GetNameSafe(AttachTo)
		);

		return;
	}

	if (WeaponClass.IsValid()) {
		HandleWeaponLoaded(WeaponClass.Get(), SlotIndex, AttachTo);
		return;
	}

	TWeakObjectPtr<UWeaponManager> WeakThis(this);

	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	Streamable.RequestAsyncLoad(
		WeaponClass.ToSoftObjectPath(),
		FStreamableDelegate::CreateLambda([WeakThis, WeaponClass, SlotIndex, AttachTo] {
			if (WeakThis.IsValid()) {
				WeakThis.Get()->HandleWeaponLoaded(WeaponClass.Get(), SlotIndex, AttachTo);
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("WeaponManager: Manager linking is Failed"));
			}
		})
	);
}

void UWeaponManager::TryToggleLaserAttachments() {
	if (!CurrentWeapon || !CurrentWeapon.Get()->GetAttachmentComponent()) return;

	CurrentWeapon.Get()->GetAttachmentComponent()->TryToggleLaserAttachments();
}

void UWeaponManager::TrySpawnCasing() {
	if (!CurrentWeapon) {
		UE_LOG(LogTemp, Warning, TEXT("WeaponBase: TrySpawnCasing - CurrentWeapon is nullptr"));
		return;
	}

	CurrentWeapon.Get()->HandleSpawnCasing();
}

void UWeaponManager::TryPlayEventSound(EWeaponAnimEvent EventType) {
	if (!CurrentWeapon) {
		UE_LOG(LogTemp, Warning, TEXT("WeaponBase: TryPlayEventSound - CurrentWeapon is nullptr"));
		return;
	}

	CurrentWeapon->HandlePlayEventSound(EventType);
}

void UWeaponManager::TryMoveWeaponStock() {
	if (CurrentState != EWeaponState::Ready) return;

	if (CurrentWeapon.Get()->GetAttachmentComponent()->HasStockAttachment()) {
		CurrentState = EWeaponState::Stocking;

		if (CurrentWeapon.Get()->GetAttachmentComponent()->IsStockExtended()) {
			CurrentWeapon.Get()->GetAttachmentComponent()->ToggleStockState(EAttachmentState::Folded);

			OnWeaponMontageStarted.Broadcast(CurrentWeapon.Get()->GetWeaponData(), EWeaponMontage::StockIn);
		}
		else {
			CurrentWeapon->GetAttachmentComponent()->ToggleStockState(EAttachmentState::Extended);

			OnWeaponMontageStarted.Broadcast(CurrentWeapon.Get()->GetWeaponData(), EWeaponMontage::StockOut);
		}
	}
}

void UWeaponManager::HandleTryChargeWeapon() {
	if (CurrentState == EWeaponState::Reloading && CurrentWeapon.Get()->GetIsNeedCharge()) {
		CurrentWeapon.Get()->SetIsNeedCharge(false);

		CurrentState = EWeaponState::Charging;

		OnWeaponMontageStarted.Broadcast(CurrentWeapon.Get()->GetWeaponData(), EWeaponMontage::Charge);

		return;
	}
	else if (CurrentWeapon.Get()->IsChargeEveryShoot() && CurrentWeapon.Get()->HasCurrentAmmo() && CurrentWeapon.Get()->GetIsNeedCharge()) {
		CurrentWeapon.Get()->SetIsNeedCharge(false);

		CurrentState = EWeaponState::Charging;
		
		OnWeaponMontageStarted.Broadcast(CurrentWeapon.Get()->GetWeaponData(), EWeaponMontage::Charge);

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
	CurrentWeapon.Get()->HandleChangeSpread();

	CurrentState = EWeaponState::Ready;
}

void UWeaponManager::HandleSetReloadState(UWeaponData*, EWeaponMontage) {
	CurrentState = EWeaponState::Reloading;

	if (StateComp) {
		StateComp->TrySetWeaponState(EWeaponState::Reloading);
	}
}

void UWeaponManager::HandleSetReadyState() {
	CurrentState = EWeaponState::Ready;

	if (StateComp) {
		StateComp->TrySetWeaponState(EWeaponState::Ready);
	}
}

void UWeaponManager::HandleReloadFinished() {
	CurrentWeapon.Get()->ReloadFinished();

	if (CurrentWeapon.Get()->GetIsNeedCharge()) {
		HandleTryChargeWeapon();

		return;
	}

	HandleSetReadyState();
}

void UWeaponManager::TryInsertAmmo() {
	if (CurrentWeapon.Get()->IsClipFull() || !CurrentWeapon.Get()->HasReserveAmmo()) {
		OnWeaponMontageStarted.Broadcast(CurrentWeapon.Get()->GetWeaponData(), EWeaponMontage::Reload_Finish);

		return;
	}
	else {
		OnWeaponMontageStarted.Broadcast(CurrentWeapon.Get()->GetWeaponData(), EWeaponMontage::InsertAmmo);
	}
}

void UWeaponManager::HandleAmmoInserted() {
	CurrentWeapon.Get()->AddAmmo(1);

	TryInsertAmmo();
}

void UWeaponManager::UnbindWeaponDelegates() {
	if (!CurrentWeapon) return;

	CurrentWeapon.Get()->OnWeaponAction.RemoveAll(this);
	CurrentWeapon.Get()->OnAmmoChanged.RemoveAll(this);
}

void UWeaponManager::BindWeaponDelegates() {
	if (!CurrentWeapon) return;

	CurrentWeapon.Get()->OnWeaponAction.AddUObject(
		this,
		&UWeaponManager::HandleMontageEvent
	);

	CurrentWeapon.Get()->OnAmmoChanged.AddUObject(
		this,
		&UWeaponManager::HandleAmmoChanged
	);
}

void UWeaponManager::DeactivateWeapon() {
	if (!CurrentWeapon) return;

	CurrentWeapon.Get()->SetActorHiddenInGame(true);

	UnbindWeaponDelegates();
}

void UWeaponManager::ActivateWeapon(AWeaponBase* Weapon) {
	CurrentWeapon = Weapon;
	CurrentWeapon.Get()->SetActorHiddenInGame(false);

	BindWeaponDelegates();

	OnWeaponChanged.Broadcast(CurrentWeapon.Get()->GetWeaponData());
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
				if (AWeaponBase* SpawnedActor = Cast<AWeaponBase>(Inv->GetSlot(WeaponToActivate)->SpawnedActor)) {
					ActivateWeapon(SpawnedActor);

					CurrentState = EWeaponState::Equipping;

					OnWeaponMontageStarted.Broadcast(CurrentWeapon.Get()->GetWeaponData(), EWeaponMontage::Equip);
					OnItemAction.Broadcast(EItemActionEvent::Equip);
				}
			}
		}
	);
}

void UWeaponManager::HandleAmmoChanged(const int32 CurrentAmmoValue, const int32 TotalAmmoValue) const {
	OnAmmoChanged.Broadcast(CurrentAmmoValue, TotalAmmoValue);
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
		HandleSetReloadState(Data, MontageType);
	}

	OnWeaponMontageStarted.Broadcast(Data, MontageType);
}

void UWeaponManager::SwapWeapon(int32 Index) {
	if (CurrentWeaponIndex == Index || CurrentState != EWeaponState::Ready) return;

	if (AWeaponBase* WeaponActor = WeaponsArr[Index].Get()) {
		PendingWeaponIndex = Index;

		if (!CurrentWeapon) {
			CurrentState = EWeaponState::Ready;

			ActivateWeapon(WeaponActor);

			OnWeaponMontageStarted.Broadcast(CurrentWeapon.Get()->GetWeaponData(), EWeaponMontage::Equip);
			OnItemAction.Broadcast(EItemActionEvent::Equip);

			return;
		}

		CurrentState = EWeaponState::Holstering;

		OnWeaponMontageStarted.Broadcast(CurrentWeapon.Get()->GetWeaponData(), EWeaponMontage::Holster);
		OnItemAction.Broadcast(EItemActionEvent::Unequip);
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

AWeaponBase* UWeaponManager::SpawnWeapon(UClass* WeaponClass, USceneComponent* AttachTo) {
	if (WeaponClass) {
		FActorSpawnParameters Params;
		Params.Owner = GetOwner();

		AWeaponBase* SpawnedActor = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, FTransform(), Params);

		if (SpawnedActor) {
			SpawnedActor->AttachToComponent(AttachTo, FAttachmentTransformRules::SnapToTargetIncludingScale, SpawnedActor->GetWeaponData()->Socket);
			SpawnedActor->SetActorHiddenInGame(true);

			WeaponsArr.Add(SpawnedActor);

			return SpawnedActor;
		}
	}

	return nullptr;
}

void UWeaponManager::TryReload() const {
	if (!CurrentWeapon || CurrentState != EWeaponState::Ready) return;
	if (StateComp && !StateComp->CanReload()) return;

	CurrentWeapon.Get()->Reload();
}

void UWeaponManager::TryShoot() const {
	if (!CurrentWeapon || CurrentState != EWeaponState::Ready || CurrentWeapon.Get()->GetIsNeedCharge()) return;
	if (StateComp && StateComp->IsSprinting()) return;

	if (CurrentWeapon.Get()->Shoot()) {
		OnShoot.Broadcast(CurrentWeapon->GetRecoil());
	}
}

void UWeaponManager::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Owner = GetOwner()) {
		StateComp = Owner->FindComponentByClass<UFPSStateComponent>();
	}
}