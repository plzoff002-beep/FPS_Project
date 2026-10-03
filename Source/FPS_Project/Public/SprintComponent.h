#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SprintComponent.generated.h"

class UStaminaComponent;
class UWeaponManager;
class UCharacterStatsDataAsset;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPS_PROJECT_API USprintComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USprintComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FORCEINLINE bool IsSprinting() const { return bSprinting; }

	void SetWantToSprint(bool bValue);
	void SetStaminaComponent(TObjectPtr<UStaminaComponent> Comp);

	void InitFromDataAsset(TObjectPtr<UCharacterStatsDataAsset> DataAsset);

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, Category = "Sprint")
		float WalkSpeed = 600.f;

	UPROPERTY(EditAnywhere, Category = "Sprint")
		float SprintSpeed = 1200.f;

	UPROPERTY(EditAnywhere, Category = "Sprint")
		float SpeedAccel = 5.f;

private:
	bool bWantToSprint = false;
	bool bSprinting = false;

	UPROPERTY() TObjectPtr<UStaminaComponent> StaminaComponent = nullptr;
	UPROPERTY() TObjectPtr<UWeaponManager> WeaponManager = nullptr;
	UPROPERTY() TObjectPtr<ACharacter> Owner = nullptr;
};
