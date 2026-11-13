#include "FPSStateComponent.h"

UFPSStateComponent::UFPSStateComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UFPSStateComponent::HandleBroadcastStateList() {
	if (OnStatesChanged.IsBound()) {
		OnStatesChanged.Broadcast(StateList);
	}
}

bool UFPSStateComponent::TrySetCloseToWall(bool bEnable) {
	if (StateList.bCloseToWall == bEnable) return false;

	StateList.bCloseToWall = bEnable;
	HandleBroadcastStateList();

	return true;
}

bool UFPSStateComponent::TrySetFalling(bool bEnable) {
	if (StateList.bFalling == bEnable) return false;

	StateList.bFalling = bEnable;
	HandleBroadcastStateList();

	return true;
}

bool UFPSStateComponent::TryToggleAimed() {
	StateList.bAimed = !StateList.bAimed;
	HandleBroadcastStateList();

	return true;
}

bool UFPSStateComponent::TrySetStanceState(EStanceState NewState) {
	if (NewState == StateList.StanceState) return false;

	StateList.StanceState = NewState;
	HandleBroadcastStateList();

	return true;
}

bool UFPSStateComponent::TrySetWeaponState(EWeaponState NewState) {
	if (NewState == StateList.WeaponState) return false;

	StateList.WeaponState = NewState;
	HandleBroadcastStateList();

	return true;
}

bool UFPSStateComponent::TrySetMovementState(EMovementState NewState) {
	if (NewState == StateList.MovementState) return false;

	if (NewState == EMovementState::Sprinting) {
		if (StateList.StanceState == EStanceState::Crouching) return false;
		if (StateList.WeaponState != EWeaponState::Ready && StateList.WeaponState != EWeaponState::None) return false;
	}

	StateList.MovementState = NewState;
	HandleBroadcastStateList();

	return true;
}
