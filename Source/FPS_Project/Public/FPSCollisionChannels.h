#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"

namespace CollisionChannels {
	/** Helps to find placeable zones on objects */
	constexpr ECollisionChannel BuildableGround = ECC_GameTraceChannel1;
	
	/** Prevents overlapping another building objects */
	constexpr ECollisionChannel BuildableObject = ECC_GameTraceChannel2;
}