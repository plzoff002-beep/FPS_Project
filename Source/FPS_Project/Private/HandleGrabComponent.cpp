#include "HandleGrabComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "InteractableInterface.h"

UHandleGrabComponent::UHandleGrabComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

bool UHandleGrabComponent::IsGrabbed() const {
	if (!PhysicsHandle) return false;

	return PhysicsHandle->GetGrabbedComponent() != nullptr;
}

bool UHandleGrabComponent::ThrowObject() {
	if (!IsGrabbed()) return false;

	UPrimitiveComponent* MeshToThrow = Cast<UPrimitiveComponent>(PhysicsHandle->GetGrabbedComponent());
	FVector ThrowDirection = ViewPoint ? ViewPoint->GetForwardVector() : FVector::ZeroVector;

	ReleaseObject();

	if (MeshToThrow) {
		MeshToThrow->AddImpulse(ThrowDirection * ThrowForce, NAME_None, true);

		return true;
	}

	return false;
}

bool UHandleGrabComponent::ReleaseObject() {
	if (!PhysicsHandle || !ViewPoint) return false;

	PhysicsHandle->ReleaseComponent();

	ViewPoint = nullptr;
	PhysicsHandle = nullptr;

	return true;
}

void UHandleGrabComponent::GrabObject(ACharacter* Interactor, UPrimitiveComponent* Target, FVector ImpactPoint) {
	if (!Interactor || !Target) return;

	PhysicsHandle = Interactor->FindComponentByClass<UPhysicsHandleComponent>();
	ViewPoint = Interactor->FindComponentByClass<UCameraComponent>();

	if (!PhysicsHandle || !ViewPoint) return;

	PhysicsHandle->GrabComponentAtLocationWithRotation(
		Target,
		NAME_None,
		ImpactPoint,
		Target->GetComponentRotation()
	);
}

void UHandleGrabComponent::BeginPlay()
{
	Super::BeginPlay();

}


void UHandleGrabComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PhysicsHandle && PhysicsHandle->GrabbedComponent && ViewPoint) {
		FVector NewLocation = ViewPoint->GetComponentLocation() + (ViewPoint->GetForwardVector() * MaxGrabDistance);

		PhysicsHandle->SetTargetLocation(NewLocation);
	}
}