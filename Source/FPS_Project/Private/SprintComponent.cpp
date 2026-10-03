#include "SprintComponent.h"
#include "StaminaComponent.h"
#include "WeaponManager.h"
#include "CharacterStatsDataAsset.h"

USprintComponent::USprintComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void USprintComponent::InitFromDataAsset(TObjectPtr<UCharacterStatsDataAsset> DataAsset) {
	if (!DataAsset) return;

	WalkSpeed = DataAsset->GetStat(ECharacterStatType::WalkSpeed);
	SprintSpeed = DataAsset->GetStat(ECharacterStatType::SprintSpeed);
	SpeedAccel = DataAsset->GetStat(ECharacterStatType::SpeedAccel);
}

void USprintComponent::SetWantToSprint(bool bValue) {
	bWantToSprint = bValue;
}

void USprintComponent::SetStaminaComponent(TObjectPtr<UStaminaComponent> Comp) {
	StaminaComponent = Comp;
}

void USprintComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Owner = Cast<ACharacter>(GetOwner())) {
		StaminaComponent = Owner->FindComponentByClass<UStaminaComponent>();
		WeaponManager = Owner->FindComponentByClass<UWeaponManager>();
	}
}

void USprintComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!Owner || !StaminaComponent) return;

	const bool bCanSprint = bWantToSprint && StaminaComponent->GetStamina() > 0.f && (!WeaponManager || !WeaponManager->IsReloading());

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
}

