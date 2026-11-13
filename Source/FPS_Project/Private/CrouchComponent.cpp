#include "CrouchComponent.h"
#include "Components/CapsuleComponent.h"
#include "FPSStateComponent.h"
#include "StateTypes.h"
#include "GameFramework/Character.h"

UCrouchComponent::UCrouchComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

}

bool UCrouchComponent::CanStand() const {
	if (!OwnerCharacter) return false;

	if (UCapsuleComponent* CapsuleComp = OwnerCharacter->GetCapsuleComponent()) {
		const float CurrentHalfHeight = CapsuleComp->GetUnscaledCapsuleHalfHeight();
		const float CurrentHalfRadius = CapsuleComp->GetUnscaledCapsuleRadius();
		
		const FVector Bottom = CapsuleComp->GetComponentLocation() - FVector(0.f, 0.f, CurrentHalfHeight);
		const FVector Center = Bottom + FVector(0.f, 0.f, StandHalfHeight);

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(OwnerCharacter);

		FCollisionShape Capsule = FCollisionShape::MakeCapsule(CurrentHalfRadius, CurrentHalfHeight);

		return !GetWorld()->OverlapBlockingTestByChannel(Center, FQuat::Identity, ECC_Pawn, Capsule, Params);
	}

	return false;
}

bool UCrouchComponent::IsHeightAchieved(const float CurrentHeight, const float TargetHeight) const {
	return FMath::IsNearlyEqual(CurrentHeight, TargetHeight);
}

void UCrouchComponent::ApplyStanceState(EStanceState NewState) {
	if (!StateComp) return;

	StateComp->TrySetStanceState(NewState);
}

void UCrouchComponent::ToggleCrouch() {
	if (!OwnerCharacter || !CanStand()) return;

	bWantsCrouch = !bWantsCrouch;
	SetComponentTickEnabled(true);
}

void UCrouchComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (OwnerCharacter = Cast<ACharacter>(GetOwner())) {
		StateComp = OwnerCharacter->FindComponentByClass<UFPSStateComponent>();
	}
}

void UCrouchComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (UCapsuleComponent* CapsuleComp = OwnerCharacter->GetCapsuleComponent()) {
		const float CurrentHeight = CapsuleComp->GetUnscaledCapsuleHalfHeight();
		const float TargetHeight = bWantsCrouch ? CrouchHalfHeight : StandHalfHeight;

		float NewHeight = FMath::FInterpTo(
			CurrentHeight,
			TargetHeight,
			DeltaTime,
			CrouchAccel
		);

		if (IsHeightAchieved(NewHeight, TargetHeight)) {
			NewHeight = TargetHeight;
			SetComponentTickEnabled(false);
		}

		if (CurrentHeight < 75.f) {
			ApplyStanceState(EStanceState::Crouching);
		}
		else {
			ApplyStanceState(EStanceState::Standing);
		}

		CapsuleComp->SetCapsuleHalfHeight(NewHeight);
	}
}
