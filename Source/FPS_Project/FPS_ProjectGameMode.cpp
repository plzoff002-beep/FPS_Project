// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPS_ProjectGameMode.h"
#include "FPS_ProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFPS_ProjectGameMode::AFPS_ProjectGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
