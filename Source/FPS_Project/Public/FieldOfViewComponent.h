#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StateTypes.h"
#include "FieldOfViewComponent.generated.h"

class UCameraComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPS_PROJECT_API UFieldOfViewComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UFieldOfViewComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void PlayerStateHandler(const FStateList& StateList);

protected:
	virtual void BeginPlay() override;

	UPROPERTY() TObjectPtr<UCameraComponent> PlayerCamera = nullptr;

	UPROPERTY(EditAnywhere, Category = "Field Of View")
		float StandardFOV = 90.f;

	UPROPERTY(EditAnywhere, Category = "Field Of View")
		float SprintFOV = 110.f;

	UPROPERTY(EditAnywhere, Category = "Field Of View")
		float AimFOV = 70.f;

	UPROPERTY(EditAnywhere, Category = "Field Of View")
		float AccelFOV = 5.f;

	float GetDesiredFOV(EMovementState MovementState, bool bAimed) const;
	void SetTargetFOV(const float Value);
	void ApplyNewStates(const FStateList& StateList);
	
	void SetPlayerCamera(UCameraComponent* NewComp);

private:
	FStateList CurrentStateList;

	float TargetFOV = -1.f;
};
