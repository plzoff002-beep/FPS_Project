// Fill out your copyright notice in the Description page of Project Settings.


#include "ViewModelComponent.h"
#include "FPSCharacter.h"
#include "WeaponBase.h"
#include "WeaponData.h"
#include "AttachmentData.h"
#include "WeaponManager.h"
#include "FPSAnimInstance.h"
#include "AttachmentTypes.h"
#include "WeaponAttachmentComponent.h" 

// Sets default values for this component's properties
UViewModelComponent::UViewModelComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UViewModelComponent::StopAnimImmediately() {
	if (USkeletalMeshComponent* CharacterArms = Owner->GetCharacterArms()) {
		if (UFPSAnimInstance* Anim = Cast<UFPSAnimInstance>(CharacterArms->GetAnimInstance())) {
			Anim->Montage_Stop(0.f);
		}
	}

	if (USkeletalMeshComponent* MeshComp = WeaponManager->GetCurrentWeapon()->WeaponMesh) {
		if (UAnimInstance* Anim = MeshComp->GetAnimInstance()) {
			Anim->Montage_Stop(0.f);
		}
	}
}

void UViewModelComponent::UpdateADS(float DeltaTime) {
	if (!Owner || !Owner->GetWeaponManager() || !Owner->GetWeaponManager()->GetCurrentWeapon()) return;

	if (const UStaticMeshComponent* SightMesh = Owner->GetWeaponManager()->GetCurrentWeapon()->GetWeaponAttachmentComponent()->AttachmentComponents.FindRef(EAttachmentType::Sight)) {
		const FTransform AimSocketTransform = SightMesh->GetSocketTransform(FName("SOCKET_AimPoint"));
		const FTransform CameraSocketTrasnform = Owner->GetCharacterArms()->GetSocketTransform(FName("CameraSocket"));

		const FVector DesiredRelativeLoc = CameraSocketTrasnform.InverseTransformPosition(AimSocketTransform.GetLocation());

		const FVector TargetPos = Owner->IsAiming() ? DesiredRelativeLoc : FVector::ZeroVector;

		const FVector NewRelativeLoc = FMath::VInterpTo(Owner->GetCameraRelativeLocation(), TargetPos, DeltaTime, ADSAccel);

		Owner->SetCameraRelativeLocation(FVector(0.f, NewRelativeLoc.Y, NewRelativeLoc.Z));
	}
}

void UViewModelComponent::HandleWeaponMontageEvent(UWeaponData* Data, EWeaponMontage MontageType) {
	USkeletalMeshComponent* CharacterArms = Owner->GetCharacterArms();

	if (CharacterArms) {
		if (UFPSAnimInstance* Anim = Cast<UFPSAnimInstance>(CharacterArms->GetAnimInstance())) {
			if (const TObjectPtr<UAnimMontage>* MontageArms = Data->MontageAnimations_Arms.Find(MontageType)) {
				Anim->Montage_Play(*MontageArms);
			}
		}
	}

	USkeletalMeshComponent* WeaponMesh = WeaponManager->GetCurrentWeapon()->WeaponMesh;

	if (WeaponMesh) {
		if (UAnimInstance* Anim = WeaponMesh->GetAnimInstance()) {
			if (const TObjectPtr<UAnimMontage>* MontageWeapon = Data->MontageAnimations_Weapon.Find(MontageType)) {
				Anim->Montage_Play(*MontageWeapon);
			}
		}
	}
}

void UViewModelComponent::HandleWeaponCasingEvent() const {
	WeaponManager->GetCurrentWeapon()->SpawnCasing();
}

void UViewModelComponent::HandleMontageFinished(EWeaponAnimStage StageType) const {
	OnMontageFinished.Broadcast(StageType);
}

void UViewModelComponent::HandleWeaponAnimEvent(EWeaponAnimEvent EventType) const {
	WeaponManager->GetCurrentWeapon()->PlayEventSound(EventType);
}

void UViewModelComponent::OnWeaponChanged(UWeaponData* Data) {
	if (USkeletalMeshComponent* Arms = Owner->GetCharacterArms()) {
		if (Arms->GetAnimInstance() && Data) {
			Arms->SetHiddenInGame(false);

			Arms->LinkAnimClassLayers(Data->ViewModelAnimBP);

			Arms->RefreshBoneTransforms();
			Arms->UpdateComponentToWorld();
		}
		else {
			Arms->SetHiddenInGame(true);

			StopAnimImmediately();
		}
	}
}

void UViewModelComponent::UpdateViewModelState() {
	State.Rotation = WeaponRotationConfig.VelocityZ.CurrentVelocity;
	State.FlightValue = WeaponHeightConfig.VelocityZ.CurrentVelocity;

	State.MovementX = WeaponMovementConfig.MovementX.CurrentVelocity;
	State.MovementY = WeaponMovementConfig.MovementY.CurrentVelocity;

	State.SwayInputX = WeaponSwayConfig.InputX.CurrentVelocity;
	State.SwayInputY = WeaponSwayConfig.InputY.CurrentVelocity;
}

void UViewModelComponent::UpdateMovementTarget(FMovementSpring& Config) {
	const float AimPenaltyX = Owner->IsAiming() ? Config.MovementX.AimPenaltyValue : 1.f;
	const float AimPenaltyY = Owner->IsAiming() ? Config.MovementY.AimPenaltyValue : 1.f;

	if (!Owner) return;

	FVector LocalVelocity = Owner->GetActorTransform().InverseTransformVectorNoScale(Owner->GetVelocity());

	if (!FMath::IsNearlyZero(LocalVelocity.X)) {
		Config.MovementX.TargetVelocity = FMath::Clamp(
			(LocalVelocity.X * Config.MovementSensivity) * AimPenaltyX * -1.f,
			Config.MovementX.VelocityClampMin,
			Config.MovementX.VelocityClampMax
		);
	}
	else {
		Config.MovementX.TargetVelocity = 0.f;
	}

	if (!FMath::IsNearlyZero(LocalVelocity.Y)) {
		Config.MovementY.TargetVelocity = FMath::Clamp(
			(LocalVelocity.Y * Config.MovementSensivity) * AimPenaltyY,
			Config.MovementY.VelocityClampMin,
			Config.MovementY.VelocityClampMax
		);
	}
	else {
		Config.MovementY.TargetVelocity = 0.f;
	}

}

void UViewModelComponent::UpdateSwayTarget(FSwaySpring& Config) {
	const float AimPenaltyX = Owner->IsAiming() ? Config.InputX.AimPenaltyValue : 1.f;
	const float AimPenaltyY = Owner->IsAiming() ? Config.InputY.AimPenaltyValue : 1.f;

	if (!FMath::IsNearlyZero(MouseInputThisFrame.X)) {
		Config.InputX.TargetVelocity = FMath::Clamp(
			MouseInputThisFrame.X * AimPenaltyX * -1.f,
			Config.InputX.VelocityClampMin,
			Config.InputX.VelocityClampMax
		);
	}
	else {
		Config.InputX.TargetVelocity = 0.f;
	}

	if (!FMath::IsNearlyZero(MouseInputThisFrame.Y)) {
		Config.InputY.TargetVelocity = FMath::Clamp(
			MouseInputThisFrame.Y * AimPenaltyY,
			Config.InputY.VelocityClampMin,
			Config.InputY.VelocityClampMax
		);
	}
	else {
		Config.InputY.TargetVelocity = 0.f;
	}
}

void UViewModelComponent::UpdateJumpTarget(FJumpSpring& Config) {
	const float AimPenalty = Owner->IsAiming() ? Config.VelocityZ.AimPenaltyValue : 1.f;

	if (Owner->IsFalling()) {
		Config.VelocityZ.TargetVelocity = FMath::Clamp(
			(Owner->GetVelocity().Z * Config.JumpSensivity) * AimPenalty,
			Config.VelocityZ.VelocityClampMin,
			Config.VelocityZ.VelocityClampMax
		);
	}
	else {
		Config.VelocityZ.TargetVelocity = 0.f;
	}
}

void UViewModelComponent::UpdateSpring(float DeltaSeconds, FFloatSpring& Config) {
	if (!Owner) {
		return;
	}

	Config.CurrentVelocity = UKismetMathLibrary::FloatSpringInterp(
		Config.CurrentVelocity,
		Config.TargetVelocity,
		Config.SpringState,
		Config.Stiffness,
		Config.CriticalDampingFactor,
		DeltaSeconds,
		Config.Mass
	);

}

// Called when the game starts
void UViewModelComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
	Owner = Cast<AFPSCharacter>(GetOwner());

	if (Owner) {
		WeaponManager = Owner->GetWeaponManager();
	}
}


// Called every frame
void UViewModelComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateJumpTarget(WeaponRotationConfig);
	UpdateSpring(DeltaTime, WeaponRotationConfig.VelocityZ);

	UpdateJumpTarget(WeaponHeightConfig);
	UpdateSpring(DeltaTime, WeaponHeightConfig.VelocityZ);

	UpdateSwayTarget(WeaponSwayConfig);
	UpdateSpring(DeltaTime, WeaponSwayConfig.InputX);
	UpdateSpring(DeltaTime, WeaponSwayConfig.InputY);

	UpdateMovementTarget(WeaponMovementConfig);
	UpdateSpring(DeltaTime, WeaponMovementConfig.MovementX);
	UpdateSpring(DeltaTime, WeaponMovementConfig.MovementY);

	if (Owner) {
		MouseInputThisFrame = Owner->GetMouseInput();
	}

	UpdateViewModelState();

	UpdateADS(DeltaTime);
}

