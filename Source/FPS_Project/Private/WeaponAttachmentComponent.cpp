#include "WeaponAttachmentComponent.h"
#include "AttachmentData.h"
#include "WeaponBase.h"
#include "AttachmentInterface.h"

UWeaponAttachmentComponent::UWeaponAttachmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

FTransform UWeaponAttachmentComponent::GetGripSocketTransform() const {
	if (AttachmentComponents.IsEmpty()) {
		return FTransform::Identity;
	}

	for (const auto& [MeshKey, MeshValue] : AttachmentComponents) {
		if (MeshKey != EAttachmentType::Grip || !IsValid(MeshValue)) {
			continue;
		}

		if (MeshValue->DoesSocketExist(FName("S_HandGrip"))) {
			FTransform GripSocketTransform = MeshValue->GetSocketTransform(FName("S_HandGrip"));

			return GripSocketTransform;
		}
	}

	return FTransform::Identity;
}

FWeaponStatModifiers UWeaponAttachmentComponent::GetTotalModifiers() const {
	FWeaponStatModifiers Result;

	if (CurrentAttachments.IsEmpty()) return {};

	for (const auto& [_, RunTimeData] : CurrentAttachments) {
		Result.AdditiveDamage += RunTimeData.Data->GetModifiers().AdditiveDamage;
		Result.AdsSpreadMultiplier *= RunTimeData.Data->GetModifiers().AdsSpreadMultiplier;
		Result.RecoilMultiplier *= RunTimeData.Data->GetModifiers().RecoilMultiplier;
		Result.SpreadMultiplier *= RunTimeData.Data->GetModifiers().SpreadMultiplier;
	}

	return Result;
}

bool UWeaponAttachmentComponent::IsStockExtended() const {
	if (!CurrentAttachments.IsEmpty()) {
		if (const FAttachmentRunTimeData* RunTimeData = CurrentAttachments.Find(EAttachmentType::Stock)) {
			return RunTimeData->Data->IsAttachmentExtended();
		}
	}

	return false;
}

bool UWeaponAttachmentComponent::HasStockAttachment() const {
	if (!CurrentAttachments.IsEmpty()) {
		for (auto& [AttachmentKey, _] : CurrentAttachments) {
			if (AttachmentKey == EAttachmentType::Stock) {
				return true;
			}
		}
	}

	return false;
}

bool UWeaponAttachmentComponent::HasGripAttachment() const {
	if (!AttachmentComponents.IsEmpty()) {
		for (const auto& It : AttachmentComponents) {
			if (IsValid(It.Value) && It.Key == EAttachmentType::Grip) {
				return true;
			}
		}
	}

	return false;
}

void UWeaponAttachmentComponent::ToggleStockState(EAttachmentState NewState) {
	if (!CurrentAttachments.IsEmpty()) {
		if (FAttachmentRunTimeData* RunTimeData = CurrentAttachments.Find(EAttachmentType::Stock)) {
			RunTimeData->Data->SetAttachmentState(NewState);
		}
	}
}

void UWeaponAttachmentComponent::SetAttachmentsHiddenInGame(bool bNewHidden) {
	if (!SpawnedAttachmentActors.IsEmpty()) {
		for (auto& It : SpawnedAttachmentActors) {
			It.Value->SetActorHiddenInGame(bNewHidden);
		}
	}
}

void UWeaponAttachmentComponent::TryToggleLaserAttachments() {
	if (SpawnedAttachmentActors.IsEmpty()) return;

	for (const auto& [ActorKey, ActorValue] : SpawnedAttachmentActors) {
		if (ActorKey != EAttachmentType::Laser) {
			continue;
		}

		if (ActorValue->Implements<UAttachmentInterface>()) {
			IAttachmentInterface::Execute_ToggleAttachment(ActorValue);
		}
	}
}

void UWeaponAttachmentComponent::AddAttachment(const FAttachmentRunTimeData& NewRunTimeData) {
	if (!NewRunTimeData.Data) return;

	if (NewRunTimeData.Data->AttachmentClass) {
		FActorSpawnParameters Params;

		Params.Owner = GetOwner();
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AActor* NewAttachmentActor = GetWorld()->SpawnActor<ALaserAttachment>(NewRunTimeData.Data->AttachmentClass, FTransform(), Params);

		if (NewAttachmentActor) {
			SpawnedAttachmentActors.Add(NewRunTimeData.Data->AttachmentType, NewAttachmentActor);

			AWeaponBase* Owner = Cast<AWeaponBase>(GetOwner());

			NewAttachmentActor->AttachToComponent(Owner->GetWeaponMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, NewRunTimeData.Data->TargetSocket);

			UStaticMeshComponent* VisualMesh = NewAttachmentActor->FindComponentByClass<UStaticMeshComponent>();

			if (VisualMesh) {
				AttachmentComponents.Add(NewRunTimeData.Data->AttachmentType, VisualMesh);
			}
		}
	}

	if (NewRunTimeData.Data->AttachmentMesh) {
		UStaticMeshComponent* NewMesh = NewObject<UStaticMeshComponent>(this);
		NewMesh->SetStaticMesh(NewRunTimeData.Data->AttachmentMesh);

		NewMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		NewMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		NewMesh->SetSimulatePhysics(false);

		FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);

		AWeaponBase* Owner = Cast<AWeaponBase>(GetOwner());

		NewMesh->AttachToComponent(Owner->GetWeaponMesh(), AttachmentRules, NewRunTimeData.Data->TargetSocket);
		NewMesh->RegisterComponent();

		AttachmentComponents.Add(NewRunTimeData.Data->AttachmentType, NewMesh);
	}
}

void UWeaponAttachmentComponent::Initialize() {
	if (!CurrentAttachments.IsEmpty()) {
		for (const auto& [_, RunTimeData] : CurrentAttachments) {
			AddAttachment(RunTimeData);
		}
	}
}

