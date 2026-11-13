#include "ViewModelComponent.h"
#include "FPSCharacter.h"
#include "WeaponBase.h"
#include "WeaponData.h"
#include "AttachmentData.h"
#include "WeaponManager.h"
#include "FPSAnimInstance.h"
#include "AttachmentTypes.h"
#include "ItemTypes.h"
#include "WeaponAttachmentComponent.h" 
#include "FPSStateComponent.h"
#include "Kismet/KismetMathLibrary.h"

UViewModelComponent::UViewModelComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UViewModelComponent::ResetCharacterArms() {
	if (!Owner.IsValid()) {
		UE_LOG(LogTemp, Error, TEXT("ViewModelComponent: ResetCharacterArms - Owner is nullptr"));
		return;
	}

	if (USkeletalMeshComponent* CharacterArms = Owner.Get()->GetCharacterArms()) {
		CharacterArms->SetRelativeLocation(FVector::ZeroVector);
		return;
	}

	UE_LOG(LogTemp, Error, TEXT("ViewModelComponent: ResetCharacterArms - Owner is nullptr"));
}

void UViewModelComponent::ItemActionHandler(EItemActionEvent Action) {
	switch (Action) {
		case EItemActionEvent::Equip:
			SetComponentTickEnabled(true);
			break;

		case EItemActionEvent::Unequip:
			ResetCharacterArms();
			ResetViewModelState();
			SetComponentTickEnabled(false);
			
			break;
	}
}

void UViewModelComponent::SetWeaponManager(UWeaponManager* NewWeaponManager) {
	WeaponManager = NewWeaponManager;
}

void UViewModelComponent::SetStateComp(UFPSStateComponent* NewStateComp) {
	StateComp = NewStateComp;
}

void UViewModelComponent::SetOwner(AFPSCharacter* NewOwner) {
	Owner = NewOwner;

	if (!Owner.IsValid()) {
		UE_LOG(LogTemp, Warning, TEXT("ViewModelComponent: SetOwner - Owner linking is failed"));
	}
}

void UViewModelComponent::StopAnimImmediately() {
	if (!Owner.IsValid()) {
		UE_LOG(LogTemp, Error, TEXT("ViewModelComponent: StopAnimImmediately - Owner is nullptr"));
		return;
	}

	if (USkeletalMeshComponent* CharacterArms = Owner.Get()->GetCharacterArms()) {
		if (UFPSAnimInstance* Anim = Cast<UFPSAnimInstance>(CharacterArms->GetAnimInstance())) {
			Anim->Montage_Stop(0.f);
		}
	}

	if (WeaponManager.IsValid() && WeaponManager.Get()->IsHoldingWeapon()) {
		if (const USkeletalMeshComponent* MeshComp = WeaponManager.Get()->GetWeaponMesh()) {
			if (UAnimInstance* Anim = MeshComp->GetAnimInstance()) {
				Anim->Montage_Stop(0.f);
			}
		}
	}
}

void UViewModelComponent::UpdateADS(float DeltaTime) {
	if (!Owner.IsValid() || !StateComp.IsValid() || !WeaponManager.IsValid()) {
		UE_LOG(LogTemp, Error, TEXT("ViewModelComponent: UpdateADS. Owner: %s, StateComp: %s, WeaponManager: %s"),
			*GetNameSafe(Owner.Get()),
			*GetNameSafe(StateComp.Get()),
			*GetNameSafe(WeaponManager.Get())
		);

		return;
	}

	if (!WeaponManager.Get()->IsHoldingWeapon()) return;

	if (const UStaticMeshComponent* SightMesh = WeaponManager.Get()->GetAttachmentMesh(EAttachmentType::Sight)) {
		const FTransform AimSocketTransform = SightMesh->GetSocketTransform(FName("SOCKET_AimPoint"));
		const FTransform CameraSocketTrasnform = Owner.Get()->GetCharacterArms()->GetSocketTransform(FName("CameraSocket"));

		const FVector DesiredRelativeLoc = CameraSocketTrasnform.InverseTransformPosition(AimSocketTransform.GetLocation());

		const FVector TargetPos = StateComp.Get()->IsAiming() ? DesiredRelativeLoc : FVector::ZeroVector;

		const FVector NewRelativeLoc = FMath::VInterpTo(Owner.Get()->GetCameraRelativeLocation(), TargetPos, DeltaTime, ADSAccel);

		Owner.Get()->SetCameraRelativeLocation(FVector(0.f, NewRelativeLoc.Y, NewRelativeLoc.Z));
	}
}

void UViewModelComponent::HandleWeaponMontageEvent(UWeaponData* Data, EWeaponMontage MontageType) {
	if (!Owner.IsValid()) {
		UE_LOG(LogTemp, Error, TEXT("ViewModelComponent: HandleWeaponMontageEvent - Owner is nullptr"));
		return;
	}

	if (USkeletalMeshComponent* CharacterArms = Owner.Get()->GetCharacterArms()) {
		if (UFPSAnimInstance* Anim = Cast<UFPSAnimInstance>(CharacterArms->GetAnimInstance())) {
			if (const TObjectPtr<UAnimMontage>* MontageArms = Data->MontageAnimations_Arms.Find(MontageType)) {
				Anim->Montage_Play(*MontageArms);
			}
		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("ViewModeComponent: HandleWeaponMontageEvent - CharacterArms is nullptr"));
	}

	if (const USkeletalMeshComponent* WeaponMesh = WeaponManager.Get()->GetWeaponMesh()) {
		if (WeaponMesh) {
			if (UAnimInstance* Anim = WeaponMesh->GetAnimInstance()) {
				if (const TObjectPtr<UAnimMontage>* MontageWeapon = Data->MontageAnimations_Weapon.Find(MontageType)) {
					Anim->Montage_Play(*MontageWeapon);
				}
			}
		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("ViewModeComponent: HandleWeaponMontageEvent - WeaponMesh is nullptr"));
	}
}

void UViewModelComponent::HandleWeaponCasingEvent() const {
	if (WeaponManager.IsValid() && WeaponManager.Get()->IsHoldingWeapon()) {
		WeaponManager.Get()->TrySpawnCasing();
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("ViewModeComponent: HandleWeaponCasingEvent. WeaponManager: %s, IsHoldingWeapon: %s"),
			*GetNameSafe(WeaponManager.Get()),
			(WeaponManager.IsValid() && WeaponManager.Get()->IsHoldingWeapon()) ? TEXT("True") : TEXT("False")
		);
	}
}

void UViewModelComponent::HandleMontageFinished(EWeaponAnimStage StageType) const {
	OnMontageFinished.Broadcast(StageType);
}

void UViewModelComponent::HandleWeaponAnimEvent(EWeaponAnimEvent EventType) const {
	if (WeaponManager.IsValid() && WeaponManager.Get()->IsHoldingWeapon()) {
		WeaponManager.Get()->TryPlayEventSound(EventType);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("ViewModeComponent: HandleWeaponAnimEvent. WeaponManager: %s, IsHoldingWeapon: %s"),
			*GetNameSafe(WeaponManager.Get()),
			(WeaponManager.IsValid() && WeaponManager.Get()->IsHoldingWeapon()) ? TEXT("True") : TEXT("False")
		);
	}
}

void UViewModelComponent::OnWeaponChanged(UWeaponData* Data) {
	if (USkeletalMeshComponent* Arms = Owner.Get()->GetCharacterArms()) {
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
	else {
		UE_LOG(LogTemp, Error, TEXT("ViewModeComponent: OnWeaponChanged. Owner: %s, Arms: %s"),
			*GetNameSafe(Owner.Get()),
			(Owner.IsValid() && Owner.Get()->GetCharacterArms()) ? TEXT("True") : TEXT("False")
		);
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
	if (!Owner.IsValid() || !StateComp.IsValid()) {
		UE_LOG(LogTemp, Error, TEXT("ViewModelComponent: UpdateMovementTarget. Owner: %s, StateComp: %s"), 
			*GetNameSafe(Owner.Get()),
			*GetNameSafe(StateComp.Get())
		);

		return;
	}

	const float AimPenaltyX = StateComp.Get()->IsAiming() ? Config.MovementX.AimPenaltyValue : 1.f;
	const float AimPenaltyY = StateComp.Get()->IsAiming() ? Config.MovementY.AimPenaltyValue : 1.f;

	FVector LocalVelocity = Owner.Get()->GetActorTransform().InverseTransformVectorNoScale(Owner.Get()->GetVelocity());

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
	if (!StateComp.IsValid()) {
		UE_LOG(LogTemp, Error, TEXT("ViewModelComponent: UpdateSwayTarget - StateComp is nullptr"));
		return;
	}

	const float AimPenaltyX = StateComp.Get()->IsAiming() ? Config.InputX.AimPenaltyValue : 1.f;
	const float AimPenaltyY = StateComp.Get()->IsAiming() ? Config.InputY.AimPenaltyValue : 1.f;

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
	if (!Owner.IsValid() || !StateComp.IsValid()) {
		UE_LOG(LogTemp, Error, TEXT("ViewModelComponent: UpdateJumpTarget. Owner: %s, StateComp: %s"),
			*GetNameSafe(Owner.Get()),
			*GetNameSafe(StateComp.Get())
		);

		return;
	}

	const float AimPenalty = StateComp.Get()->IsAiming() ? Config.VelocityZ.AimPenaltyValue : 1.f;

	const float CurrentVelocity = Owner.Get()->GetVelocity().Z;

	Config.VelocityZ.TargetVelocity = FMath::Clamp(
		(CurrentVelocity * Config.JumpSensivity) * AimPenalty,
		Config.VelocityZ.VelocityClampMin,
		Config.VelocityZ.VelocityClampMax
	);
}

void UViewModelComponent::UpdateSpring(float DeltaSeconds, FFloatSpring& Config) {
	if (!Owner.IsValid()) {
		UE_LOG(LogTemp, Error, TEXT("ViewModelComponent: UpdateJumpTarget - Owner is nullptr"));
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

void UViewModelComponent::BeginPlay()
{
	Super::BeginPlay();
	
	SetOwner(Cast<AFPSCharacter>(GetOwner()));
	
	if (Owner.IsValid()) {
		SetWeaponManager(Owner.Get()->FindComponentByClass<UWeaponManager>());
		SetStateComp(Owner.Get()->FindComponentByClass<UFPSStateComponent>());
	}
}

void UViewModelComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Owner.IsValid()) {
		MouseInputThisFrame = Owner.Get()->GetMouseInputThisFrame();
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("ViewModelComponent: TickComponent - Owner is nullptr"));
	}

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

	UpdateViewModelState();

	UpdateADS(DeltaTime);
}

