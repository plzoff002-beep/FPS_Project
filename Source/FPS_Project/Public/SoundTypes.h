#pragma once

#include "SoundTypes.generated.h"

UENUM(BlueprintType)
enum class ESoundClassType : uint8 {
	None,
	Master,
	Music
};