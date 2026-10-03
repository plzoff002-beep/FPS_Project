#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StaminaComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnStaminaChanged, const float);

class UCharacterStatsDataAsset;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPS_PROJECT_API UStaminaComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UStaminaComponent();

	FORCEINLINE float GetStamina() const { return CurrentStamina; }
	FORCEINLINE float GetPercentage() const { return CurrentStamina / MaxStamina; }

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Drain(float DeltaTime);
	void Regen(float DeltaTime);

	void InitFromDataAsset(TObjectPtr<UCharacterStatsDataAsset> DataAsset);

	FOnStaminaChanged OnStaminaChanged;

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, Category = "Logic")
		float MaxStamina = 100.f;

	UPROPERTY(EditAnywhere, Category = "Logic")
		float DrainRate = 15.f;

	UPROPERTY(EditAnywhere, Category = "Logic")
		float RegenRate = 10.f;

private:
	void BroadcastIfChanged();

	float CurrentStamina = MaxStamina;
	float PreviousStamina = MaxStamina;
};
