#include "SprintComponent.h"
#include "StaminaComponent.h"
#include "FPSStateComponent.h"
#include "CharacterStatsDataAsset.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

USprintComponent::USprintComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

}

void USprintComponent::InitFromDataAsset(TObjectPtr<UCharacterStatsDataAsset> DataAsset) {
	if (!DataAsset) return;

	WalkSpeed = DataAsset->GetStat(ECharacterStatType::WalkSpeed);
	SprintSpeed = DataAsset->GetStat(ECharacterStatType::SprintSpeed);
	SpeedAccel = DataAsset->GetStat(ECharacterStatType::SpeedAccel);
}

void USprintComponent::ToggleSprint() {
	bWantToSprint = !bWantToSprint;
	SetComponentTickEnabled(true);
}

void USprintComponent::SetStaminaComponent(TObjectPtr<UStaminaComponent> Comp) {
	StaminaComponent = Comp;
}

void USprintComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Owner = Cast<ACharacter>(GetOwner())) {
		StaminaComponent = Owner->FindComponentByClass<UStaminaComponent>();
		StateComp = Owner->FindComponentByClass<UFPSStateComponent>();
	}
}

void USprintComponent::ApplyMovementState(EMovementState NewState) {
	if (!StateComp) return;

	StateComp->TrySetMovementState(NewState);
}

void USprintComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (!Owner || !StaminaComponent || !StateComp) {
		SetComponentTickEnabled(false);
		return;
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const bool bCanSprint = bWantToSprint && StaminaComponent->GetStamina() > 0.f && 
		!StateComp->IsBusy() && StateComp->IsStanding();

	const float TargetSpeed = bCanSprint ? SprintSpeed : WalkSpeed;
	float& CurrentSpeed = Owner->GetCharacterMovement()->MaxWalkSpeed;

	CurrentSpeed = FMath::FInterpTo(CurrentSpeed, TargetSpeed, DeltaTime, SpeedAccel);

	if (Owner->GetVelocity().Size2D() > WalkSpeed + 15.f) {
		bSprinting = true;
		StaminaComponent->Drain(DeltaTime);
	}
	else {
		bSprinting = false;
		StaminaComponent->Regen(DeltaTime);
	}

	if (FMath::IsNearlyEqual(CurrentSpeed, TargetSpeed)) {
		SetComponentTickEnabled(false);
	}
}

