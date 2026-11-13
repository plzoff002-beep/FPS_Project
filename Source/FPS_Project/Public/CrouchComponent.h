#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CrouchComponent.generated.h"

class ACharacter;
class UFPSStateComponent;

enum class EStanceState : uint8;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPS_PROJECT_API UCrouchComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCrouchComponent();

	void ToggleCrouch();
	
	FORCEINLINE bool GetCrouchState() const { return bWantsCrouch; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Crouch")
		float CrouchHalfHeight = 44.f;

	UPROPERTY(EditAnywhere, Category = "Crouch")
		float StandHalfHeight = 88.f;

	UPROPERTY(EditAnywhere, Category = "Crouch")
		float CrouchAccel = 5.f;

private:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY() TObjectPtr<ACharacter> OwnerCharacter = nullptr;
	UPROPERTY() TObjectPtr<UFPSStateComponent> StateComp = nullptr;

	bool bWantsCrouch = false;

	bool CanStand() const;
	bool IsHeightAchieved(const float CurrentHeight, const float TargetHeight) const;

	void ApplyStanceState(EStanceState NewState);
};
