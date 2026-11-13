#include "InventoryComponent.h"
#include "ItemDefinition.h"
#include "FPSCharacter.h"
#include "WeaponBase.h"
#include "WeaponData.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

FInventorySlot* UInventoryComponent::GetSlot(const int32 SlotIndex) {
	if (!Slots.IsValidIndex(SlotIndex) || !Slots[SlotIndex].ItemDef || Slots[SlotIndex].Quantity <= 0) {
		return nullptr;
	}

	return &Slots[SlotIndex];
}

void UInventoryComponent::DropItem(const int32 SlotIndex) {
	if (!Slots.IsValidIndex(SlotIndex) || !Slots[SlotIndex].ItemDef || Slots[SlotIndex].Quantity <= 0) {
		return;
	}

	FInventorySlot SlotInfo = {};

	SlotInfo.ItemDef = Slots[SlotIndex].ItemDef;
	SlotInfo.Quantity = 1;

	Slots[SlotIndex].Quantity -= 1;

	if (Slots[SlotIndex].Quantity <= 0) {
		ClearSlot(Slots[SlotIndex]);
	}

	OnInventoryChanged.Broadcast(SlotIndex);
	OnInventorySlotDropped.Broadcast(SlotInfo);
}

void UInventoryComponent::RemoveItem(const int32 SlotIndex) {
	if (!Slots.IsValidIndex(SlotIndex) || !Slots[SlotIndex].ItemDef || Slots[SlotIndex].Quantity <= 0) {
		return;
	}
		FInventorySlot SlotInfo = {};

		SlotInfo.ItemDef = Slots[SlotIndex].ItemDef;
		SlotInfo.Quantity = 1;

		Slots[SlotIndex].Quantity -= 1;

		if (Slots[SlotIndex].Quantity <= 0) {
			ClearSlot(Slots[SlotIndex]);
		}

		OnInventoryChanged.Broadcast(SlotIndex);
}

bool UInventoryComponent::AddItem(UItemDefinition* Item, int32 Amount) {
	if (!Item || Amount <= 0) return false;

	if (Item->MaxStackSize > 1) {
		for (FInventorySlot& Slot : Slots) {
			if (Slot.ItemDef == Item && Slot.Quantity < Item->MaxStackSize) {
				int32 CanAdd = FMath::Min(Amount, Item->MaxStackSize - Slot.Quantity);

				Slot.Quantity += CanAdd;
				Amount -= CanAdd;

				if (Amount <= 0) {
					int32 SlotIndex = &Slot - Slots.GetData();

					OnInventoryChanged.Broadcast(SlotIndex);

					return true;
				}
			}
		}
	}

	for (FInventorySlot& Slot : Slots) {
		if (Slot.IsEmpty()) {
			Slot.ItemDef = Item;
			Slot.Quantity = Amount;
			Slot.ItemType = Item->ItemType;
			Slot.ActionMap = &Item->ActionMap;

			int32 SlotIndex = &Slot - Slots.GetData();

			OnInventoryChanged.Broadcast(SlotIndex);
			OnItemAdded.Broadcast(Item, SlotIndex);

			return true;
		}
	}

	return false;
}

void UInventoryComponent::SelectItem(const int32 Index) {
	FInventorySlot* Slot = GetSlot(Index);

	UE_LOG(LogTemp, Warning, TEXT("1"));

	if (Slot && !Slot->IsEmpty()) {
		UE_LOG(LogTemp, Warning, TEXT("2"));

		if (AActor* SpawnedActor = Slot->SpawnedActor.Get()) {
			UE_LOG(LogTemp, Warning, TEXT("3"));

			EItemType ItemType = Slot->ItemType;
			OnInventorySlotSelected.Broadcast(Index, SpawnedActor, ItemType);
		}
	}
}

void UInventoryComponent::RegisterActorInSlot(const FLoadedItemData& LoadedData) {
	if (!Slots.IsValidIndex(LoadedData.SlotIndex)) return;

	Slots[LoadedData.SlotIndex].SpawnedActor = LoadedData.LoadedActor;
}

void UInventoryComponent::ClearSlot(FInventorySlot& Slot) {
	Slot.ItemDef = nullptr;
	Slot.Quantity = 0;
	Slot.ItemType = EItemType::None;
	Slot.ActionMap = nullptr;

	int32 SlotIndex = &Slot - Slots.GetData();

	OnInventorySlotCleared.Broadcast(SlotIndex);

	if (Slot.SpawnedActor.IsValid()) {
		Slot.SpawnedActor->Destroy();
	}

	Slot.SpawnedActor = nullptr;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	Slots.SetNum(InventorySize);
}


void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

