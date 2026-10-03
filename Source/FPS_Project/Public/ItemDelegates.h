#pragma once

#include "ItemTypes.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnInventoryChanged, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnInventorySlotCleared, int32);