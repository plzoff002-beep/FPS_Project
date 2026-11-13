#include "FieldOfViewComponent.h"
#include "Camera/CameraComponent.h"

namespace {
	constexpr float FOV_TOLERANCE = 0.1f;
}

UFieldOfViewComponent::UFieldOfViewComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UFieldOfViewComponent::SetPlayerCamera(UCameraComponent* NewComp) {
	PlayerCamera = NewComp;
	if (PlayerCamera) {
		const float NewFOV = GetDesiredFOV(CurrentStateList.MovementState, CurrentStateList.bAimed);
		SetTargetFOV(NewFOV);
	}
}

void UFieldOfViewComponent::ApplyNewStates(const FStateList& StateList) {
	CurrentStateList.MovementState = StateList.MovementState;
	CurrentStateList.bAimed = StateList.bAimed;
}

float UFieldOfViewComponent::GetDesiredFOV(EMovementState MovementState, bool bAimed) const {
	if (bAimed) return AimFOV;
	else if (MovementState == EMovementState::Sprinting) return SprintFOV;
	return StandardFOV;
}

void UFieldOfViewComponent::SetTargetFOV(const float Value) {
	TargetFOV = Value;
	SetComponentTickEnabled(true);
}

void UFieldOfViewComponent::PlayerStateHandler(const FStateList& StateList) {
	if (!PlayerCamera) {
		UE_LOG(LogTemp, Warning, TEXT("FieldOfViewComponent: PlayerCamera is not Found!"));

		SetComponentTickEnabled(false);
		return;
	}

	const EMovementState MovementState = StateList.MovementState;
	const bool bAimed = StateList.bAimed;

	if (CurrentStateList.MovementState == MovementState && CurrentStateList.bAimed == bAimed) return;

	const float NewFOV = GetDesiredFOV(MovementState, bAimed);

	SetTargetFOV(NewFOV);
	ApplyNewStates(StateList);
}

void UFieldOfViewComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (AActor* Owner = GetOwner()) {
		if (UCameraComponent* Camera = Owner->FindComponentByClass<UCameraComponent>()) {
			SetPlayerCamera(Camera);
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("FieldOfViewComponent: Camera is not Found!"));
		}
	}
}

void UFieldOfViewComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PlayerCamera) {
		UE_LOG(LogTemp, Warning, TEXT("FieldOfViewComponent: PlayerCamera is not Found!"));

		SetComponentTickEnabled(false);
		return;
	}

	const float CurrentFOV = PlayerCamera->FieldOfView;

	if (FMath::IsNearlyEqual(CurrentFOV, TargetFOV, FOV_TOLERANCE)) {
		PlayerCamera->SetFieldOfView(TargetFOV);

		SetComponentTickEnabled(false);
		return;
	}

	const float NewFOV = FMath::FInterpTo(
		CurrentFOV,
		TargetFOV,
		DeltaTime,
		AccelFOV
	);

	PlayerCamera->SetFieldOfView(NewFOV);
}

