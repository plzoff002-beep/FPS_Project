// Fill out your copyright notice in the Description page of Project Settings.

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
	// Sets default values for this component's properties
	UInventoryComponent();

	FOnInventoryChanged OnInventoryChanged;
	FOnInventorySlotCleared OnInventorySlotCleared;

	bool AddItem(UItemDefinition* Item, int32 Amount = 1);

	void RegisterActorInSlot(int32 SlotIndex, AActor* SpawnedActor);

	FInventorySlot RemoveItem(int32 SlotIndex);
	FInventorySlot* GetSlot(int32 SlotIndex);

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
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Logic")
		int32 InventorySize = 20;

	UPROPERTY(VisibleAnywhere, Category = "Logic")
		TArray<FInventorySlot> Slots;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void ClearSlot(FInventorySlot& Slot);

};
