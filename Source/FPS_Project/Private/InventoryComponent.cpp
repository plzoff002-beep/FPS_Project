// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "ItemDefinition.h"
#include "FPSCharacter.h"
#include "WeaponBase.h"
#include "WeaponData.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

FInventorySlot* UInventoryComponent::GetSlot(int32 SlotIndex) {
	if (!Slots.IsValidIndex(SlotIndex) || !Slots[SlotIndex].ItemDef || Slots[SlotIndex].Quantity <= 0) {
		return nullptr;
	}

	return &Slots[SlotIndex];
}

FInventorySlot UInventoryComponent::RemoveItem(int32 SlotIndex) {
	if (!Slots.IsValidIndex(SlotIndex) || !Slots[SlotIndex].ItemDef || Slots[SlotIndex].Quantity <= 0) {
		return FInventorySlot();
	}
		FInventorySlot SlotInfo = {};

		SlotInfo.ItemDef = Slots[SlotIndex].ItemDef;
		SlotInfo.Quantity = 1;

		Slots[SlotIndex].Quantity -= 1;

		if (Slots[SlotIndex].Quantity <= 0) {
			ClearSlot(Slots[SlotIndex]);
		}

		OnInventoryChanged.Broadcast(SlotIndex);

		return SlotInfo;
}

//TArray<FInventorySlot>& UInventoryComponent::GetSlots() const {
//
//}

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

			return true;
		}
	}

	return false;
}

void UInventoryComponent::RegisterActorInSlot(int32 SlotIndex, AActor* SpawnedActor) {
	if (!Slots.IsValidIndex(SlotIndex)) return;

	Slots[SlotIndex].SpawnedActor = SpawnedActor;
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

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	Slots.SetNum(InventorySize);
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

