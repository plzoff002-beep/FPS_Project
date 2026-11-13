#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemTypes.h"
#include "ItemDelegates.h"
#include "InventoryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPS_PROJECT_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();

	FOnItemAdded OnItemAdded;

	FOnInventoryChanged OnInventoryChanged;
	FOnInventorySlotDropped OnInventorySlotDropped;
	FOnInventorySlotCleared OnInventorySlotCleared;
	FOnInventorySlotSelected OnInventorySlotSelected;

	bool AddItem(UItemDefinition* Item, int32 Amount = 1);

	void RegisterActorInSlot(const FLoadedItemData& LoadedData);

	void SelectItem(const int32 Index);

	void RemoveItem(const int32 SlotIndex);
	void DropItem(const int32 SlotIndex);

	FInventorySlot* GetSlot(const int32 SlotIndex);

	TArray<FInventorySlot>& GetSlots() { return Slots; }

	template<typename T>
	TArray<FInventorySlot*> FindSlotsWithFragment() const {
		TArray<FInventorySlot*> Result;

		for (FInventorySlot& Slot : Slots) {
			if (!Slot.IsEmpty() && Slot.ItemDef->FindFragment<T>()) {
				Result.Add(&Slot);
			}
		}

		return Result;
	}

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Logic")
		int32 InventorySize = 20;

	UPROPERTY(VisibleAnywhere, Category = "Logic")
		TArray<FInventorySlot> Slots;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void ClearSlot(FInventorySlot& Slot);

};
