#include "RecoilComponent.h"

namespace {
	constexpr float RECOIL_TOLERANCE = 0.1f;
}

URecoilComponent::URecoilComponent()
{

	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void URecoilComponent::SetPlayerController(APlayerController* NewPC) {
	PC = NewPC;

	if (!PC.IsValid()) {
		UE_LOG(LogTemp, Warning, TEXT("RecoilComponent: PlayerController linking is Failed"));
	}
}

void URecoilComponent::ResetRecoilComponent() {
	AppliedRecoilOffset = FRotator::ZeroRotator;
	TargetRecoilOffset = FRotator::ZeroRotator;

	SetComponentTickEnabled(false);
}

void URecoilComponent::AddRecoil(const float Value) {
	TargetRecoilOffset.Pitch -= Value;
	SetComponentTickEnabled(true);
}

void URecoilComponent::BeginPlay()
{
	Super::BeginPlay();	

	APlayerController* NewPC = Cast<APlayerController>(GetOwner()->GetInstigatorController());
	SetPlayerController(NewPC);
}

void URecoilComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PC.IsValid()) {
		UE_LOG(LogTemp, Error, TEXT("RecoilComponent: PlayerController is nullptr"));
		ResetRecoilComponent();

		return;
	}

	FRotator NewOffset = FMath::RInterpTo(AppliedRecoilOffset, TargetRecoilOffset, DeltaTime, RecoilAccel);
	FRotator DeltaRecoil = NewOffset - AppliedRecoilOffset;

	PC.Get()->AddPitchInput(DeltaRecoil.Pitch);

	AppliedRecoilOffset += NewOffset;

	if (AppliedRecoilOffset.Equals(TargetRecoilOffset, RECOIL_TOLERANCE)) {
		ResetRecoilComponent();
		return;
	}
}

