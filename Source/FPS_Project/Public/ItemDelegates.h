#pragma once

class UItemDefinition;

enum class EItemType : uint8;
enum class EItemActionEvent : uint8;

struct FLoadedItemData;
struct FInventorySlot;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnItemAdded, UItemDefinition*, const int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemLoaded, const FLoadedItemData&);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnInventoryChanged, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnInventorySlotDropped, FInventorySlot&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnInventorySlotCleared, int32);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnInventorySlotSelected, int32, AActor*, EItemType);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemAction, EItemActionEvent)