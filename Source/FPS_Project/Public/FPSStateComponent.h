#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StateTypes.h"
#include "StateDelegates.h"
#include "FPSStateComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPS_PROJECT_API UFPSStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UFPSStateComponent();

	FOnStatesChanged OnStatesChanged;

	bool TrySetStanceState(EStanceState NewState);
	bool TrySetMovementState(EMovementState NewState);
	bool TrySetWeaponState(EWeaponState NewState);
	bool TryToggleAimed();
	bool TrySetFalling(bool bEnable);
	bool TrySetCloseToWall(bool bEnable);

	FORCEINLINE bool CanReload() const {
		EMovementState S = StateList.MovementState;
		return (S == EMovementState::Idle || S == EMovementState::Walking);
	}

	FORCEINLINE bool IsBusy() const { 
		EWeaponState S = StateList.WeaponState;
		return !(S == EWeaponState::Ready || S == EWeaponState::None);
	}

	FORCEINLINE bool IsStanding() const { return StateList.StanceState == EStanceState::Standing; }
	FORCEINLINE bool IsAiming() const { return StateList.bAimed; }
	FORCEINLINE bool IsSprinting() const { return StateList.MovementState == EMovementState::Sprinting; }

	UFUNCTION(BlueprintPure)
		FStateList& GetStateList() { return StateList; }

protected:
	void HandleBroadcastStateList();

private:	
	FStateList StateList;

};
