#pragma once

class UObject;

enum class EButtonType : uint8;
enum class EInventoryAction : uint8;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnButtonClicked, EButtonType, UObject*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnGameplayActionRequest, EButtonType, UObject*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSlotClicked, EButtonType);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnQualityLevelChanged, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnActionSelected, EInventoryAction);