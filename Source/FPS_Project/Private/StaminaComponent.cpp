#include "StaminaComponent.h"
#include "CharacterTypes.h"
#include "CharacterStatsDataAsset.h"

UStaminaComponent::UStaminaComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UStaminaComponent::InitFromDataAsset(TObjectPtr<UCharacterStatsDataAsset> DataAsset) {
	if (!DataAsset) return;

	DrainRate = DataAsset->GetStat(ECharacterStatType::StaminaDrainRate);
	RegenRate = DataAsset->GetStat(ECharacterStatType::StaminaRegenRate);
}

void UStaminaComponent::BroadcastIfChanged() {
	if (!FMath::IsNearlyEqual(CurrentStamina, PreviousStamina)) {
		OnStaminaChanged.Broadcast(GetPercentage());

		PreviousStamina = CurrentStamina;
	}
}

void UStaminaComponent::Drain(float DeltaTime) {
	CurrentStamina = FMath::Max(0.f, CurrentStamina - (DrainRate * DeltaTime));

	BroadcastIfChanged();
}

void UStaminaComponent::Regen(float DeltaTime) {
	CurrentStamina = FMath::Min(MaxStamina, CurrentStamina + (RegenRate * DeltaTime));

	BroadcastIfChanged();
}

// Called when the game starts
void UStaminaComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UStaminaComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

