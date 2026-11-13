#include "InteractionComponent.h"
#include "ExaminationComponent.h"
#include "HandleGrabComponent.h"
#include "GameFramework/Character.h"
#include "InteractableInterface.h"
#include "ExaminatableInterface.h"
#include "DrawDebugHelpers.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UInteractionComponent::EndInteraction(const float ElapsedTime) {
	if (OwnerActor) {
		if (ElapsedTime < 0.2f) {
			if (UHandleGrabComponent* GrabComp = OwnerActor->FindComponentByClass<UHandleGrabComponent>()) {
				GrabComp->ReleaseObject();
			}

			TryPickUpObject();
		}
		else {
			TryGrabObject();
		}
	}
}

void UInteractionComponent::TryExamineItem(bool& bExamed) {
	if (!bExamed) {
		FHitResult Hit;

		if (PerformInteractionTrace(Hit)) {
			if (AActor* HitActor = Hit.GetActor()) {
				AActor* TargetActor = HitActor->GetAttachParentActor() ? 
					HitActor->GetAttachParentActor() : HitActor->GetOwner() ? 
					HitActor->GetOwner() : HitActor;

				if (IExaminatableInterface* Examinatable = Cast<IExaminatableInterface>(TargetActor)) {
					if (UExaminationComponent* ExaminationComp = OwnerActor->FindComponentByClass<UExaminationComponent>()) {
						ExaminationComp->StartExamination(Examinatable->GetExaminationItem());
						bExamed = true;
					}
				}
			}
		}
	}
	else {
		if (UExaminationComponent* ExaminationComp = OwnerActor->FindComponentByClass<UExaminationComponent>()) {
			ExaminationComp->StopExamination();
			bExamed = false;
		}
	}
}

void UInteractionComponent::TryPickUpObject() {
	if (OwnerActor) {

		FHitResult Hit;
		if (PerformInteractionTrace(Hit)) {
			if (AActor* HitActor = Hit.GetActor()) {
				if (AActor* PickUpActor = HitActor->GetOwner()) {
					if (PickUpActor && PickUpActor->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass())) {
						IInteractableInterface::Execute_Interact(PickUpActor, OwnerActor);
					}
				}
			}
		}
	}
}

void UInteractionComponent::TryGrabObject() {
	if (!OwnerActor) return;

	FHitResult Hit;
	if (PerformInteractionTrace(Hit)) {
		if (AActor* HitActor = Hit.GetActor()) {
			if (IInteractableInterface* Interactable = Cast<IInteractableInterface>(HitActor->GetOwner())) {
				UPrimitiveComponent* TargetToGrab = Interactable->GetPrimitiveOnInteract();
				
				if (ACharacter* CharacterOwner = Cast<ACharacter>(OwnerActor)) {
					if (UHandleGrabComponent* GrabComp = OwnerActor->FindComponentByClass<UHandleGrabComponent>()) {
						GrabComp->GrabObject(CharacterOwner, TargetToGrab, Hit.ImpactPoint);
					}
				}
			}
		}
	}
}

bool UInteractionComponent::PerformInteractionTrace(FHitResult& Hit) {
	if (OwnerActor) {
		FVector Origin = FVector::ZeroVector;
		FRotator Rotation = FRotator::ZeroRotator;

		OwnerActor->GetActorEyesViewPoint(Origin, Rotation);

		const FVector End = Origin + Rotation.Vector() * InteractionMaxDistance;

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(OwnerActor);

		DrawDebugLine(GetWorld(), Origin, End, FColor::Red, false, 2.f, 0, 2.f);

		return GetWorld()->LineTraceSingleByChannel(Hit, Origin, End, ECC_Visibility, Params);
	}

	return false;
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OwnerActor = GetOwner();
}

