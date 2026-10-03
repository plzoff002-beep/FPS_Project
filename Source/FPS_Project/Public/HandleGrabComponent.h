#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HandleGrabComponent.generated.h"

class UPhysicsHandleComponent;
//class UCameraComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPS_PROJECT_API UHandleGrabComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHandleGrabComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void GrabObject(ACharacter* Interactor, UPrimitiveComponent* Target, FVector ImpactPoint);
	bool ReleaseObject();
	bool ThrowObject();

	bool IsGrabbed() const;

	FORCEINLINE float GetMaxGrabDistance() const { return MaxGrabDistance; }

protected:
	virtual void BeginPlay() override;

private:
	float MaxGrabDistance = 250.f;
	float ThrowForce = 250.f;
		
	UPhysicsHandleComponent* PhysicsHandle = nullptr;
	USceneComponent* ViewPoint = nullptr;
};
