#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WeaponManager.h"
#include "WeaponIKTypes.h"
#include "CharacterTypes.h"
#include "CharacterStateProvider.h"
#include "InventoryComponent.h"
#include "SettingsTypes.h"
#include "CharacterStatsDataAsset.h"
#include "FPSCharacter.generated.h"

class UCameraComponent;
class UViewModelComponent;
class UFPSWidget;
class AWeaponBase;
class UWeaponManager;
class ULocalUIManager;
class UInventoryComponent;
class UInventoryMenuWidget;
class UUIData;
class USystemData;
class USprintComponent;
class UStaminaComponent;
class UHandleGrabComponent;
class UConstructionComponent;
class UBuildLibraryData;
class UExaminationComponent;
class UInteractionComponent;
class UCrouchComponent;
class UFPSStateComponent;
class UFieldOfViewComponent;
class URecoilComponent;

enum class EButtonType : uint8;

UCLASS()
class FPS_PROJECT_API AFPSCharacter : public ACharacter, public IWeaponIKProvider, public ICharacterStateProvider
{
	GENERATED_BODY()

public:
	AFPSCharacter();

	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;
	virtual void OnMovementModeChanged(EMovementMode Prev, uint8 PrevCustom) override;
	virtual FViewModelState GetViewModelState() override;

	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }

	FORCEINLINE FVector2D GetMouseInputThisFrame() const { return MouseInputThisFrame; }

	FORCEINLINE USkeletalMeshComponent* GetCharacterArms() const { return CharacterArms; }

	void SetPlayerGameplayConfig(FGameplaySettingsData& NewData);

	FVector GetCameraRelativeLocation() const;

	FTransform GetRightHandIKTransform() override;
	virtual FWeaponIKData GetWeaponIKData() override;

	void SetCameraRelativeLocation(FVector NewLocation);

	void UpdateIsCloseToWall() const;

	void MoveCharacter(const FVector2D& Axis);
	void Look(const FVector2D& Axis);

protected:
	virtual void BeginPlay() override;

	void ItemSelectionHandler(const int32 Index, AActor* SpawnedActor, EItemType ItemType);
	void ItemSpawnerHandler(UItemDefinition* ItemDef, const int32 SlotIndex);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TObjectPtr<USceneComponent> Pivot = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TObjectPtr<UCameraComponent> Camera = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TObjectPtr<USkeletalMeshComponent> CharacterArms = nullptr;
		
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TObjectPtr<UViewModelComponent> ViewModel = nullptr;

	UPROPERTY(EditAnywhere)
		TObjectPtr<UWeaponManager> WeaponManager = nullptr;

	UPROPERTY(EditAnywhere)
		TObjectPtr<URecoilComponent> RecoilComponent = nullptr;

	UPROPERTY(EditAnywhere)
		TObjectPtr<UFieldOfViewComponent> FOVComp = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FName CameraSocket = NAME_None;

	UPROPERTY() TObjectPtr<ULocalUIManager> UIManager = nullptr;
	UPROPERTY() TObjectPtr<UHandleGrabComponent> HandleGrabComponent = nullptr;

	UPROPERTY(EditAnywhere, Category = "Examination")
		TObjectPtr<UExaminationComponent> ExaminationComponent = nullptr;

	UPROPERTY(EditAnywhere, Category = "Interact")
		TObjectPtr<UInteractionComponent> InteractionComponent = nullptr;

	UPROPERTY(EditAnywhere, Category = "Crouch")
		TObjectPtr<UCrouchComponent> CrouchComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Build")
		TObjectPtr<UConstructionComponent> ConstructionComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "State")
		TObjectPtr<UFPSStateComponent> StateComponent = nullptr;

	UPROPERTY(EditAnywhere, Category = "Sprint")
		TObjectPtr<USprintComponent> SprintComponent = nullptr;

	UPROPERTY(EditAnywhere, Category = "Stamina")
		TObjectPtr<UStaminaComponent> StaminaComponent = nullptr;

	UPROPERTY(EditAnywhere, Category = "Build | Data")
	TObjectPtr<UBuildLibraryData> BuildLibraryData = nullptr;

	UPROPERTY(EditAnywhere, Category = "Inventory")
		TObjectPtr<UInventoryComponent> Inventory = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Data")
		TObjectPtr<USystemData> SystemData = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Data")
		TObjectPtr<UUIData> UIData = nullptr;

	UPROPERTY(EditDefaultsOnly)
		float DistanceToWall = 250.f;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	FPlayerGameplayConfig PlayerGameplayConfig = {};

	FVector2D MouseInputThisFrame;

	void UnifiedUIHandler(EButtonType ButtonType, UObject* Context);

	void UpdateMouseInputThisFrame();
	void UpdateMovementState();

	//void HandleInventoryChanged(int32 SlotIndex) const;
	void HandleSlotCleared(int32 SlotIndex);

	void SetDropLocation(FInventorySlot& ItemToDrop);
};
