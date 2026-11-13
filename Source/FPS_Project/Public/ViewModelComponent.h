#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponTypes.h"
#include "ViewModelTypes.h"
#include "WeaponDelegates.h"
#include "ViewModelComponent.generated.h"

class AFPSCharacter;
class UWeaponData;
class UWeaponManager;
class UFPSStateComponent;

enum class EItemActionEvent : uint8;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPS_PROJECT_API UViewModelComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UViewModelComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FOnMontageFinished OnMontageFinished;

	FORCEINLINE FViewModelState GetState() const { return State; }
	void SetOwner(AFPSCharacter* NewOwner);
	void SetWeaponManager(UWeaponManager* NewWeaponManager);
	void SetStateComp(UFPSStateComponent* NewStateComp);

	void HandleMontageFinished(EWeaponAnimStage StageType) const;

	void OnWeaponChanged(UWeaponData* Data);

	void HandleWeaponAnimEvent(EWeaponAnimEvent EventType) const;
	void HandleWeaponMontageEvent(UWeaponData* Data, EWeaponMontage MontageType);
	void HandleWeaponCasingEvent() const;

	void ItemActionHandler(EItemActionEvent Action);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FJumpSpring WeaponRotationConfig;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FJumpSpring WeaponHeightConfig;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FSwaySpring WeaponSwayConfig;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FMovementSpring WeaponMovementConfig;

	UPROPERTY(BlueprintReadOnly)
	FVector2D MouseInputThisFrame = (0, 0);

	TWeakObjectPtr<AFPSCharacter> Owner;

	TWeakObjectPtr<UFPSStateComponent> StateComp;
	TWeakObjectPtr<UWeaponManager> WeaponManager;

	UPROPERTY(EditAnywhere, Category = "Logic")
		float ADSAccel = 5.f;

	void UpdateADS(float DeltaTime);
	void UpdateViewModelState();

	void UpdateSpring(float DeltaTime, FFloatSpring& Config);
	void UpdateJumpTarget(FJumpSpring& Config);
	void UpdateSwayTarget(FSwaySpring& Config);
	void UpdateMovementTarget(FMovementSpring& Config);

	void StopAnimImmediately();
	void ResetCharacterArms();
	inline void ResetViewModelState() { State = FViewModelState(); }

	FViewModelState State = {};

	EWeaponMontage CurrentMontage = EWeaponMontage::None;
};
