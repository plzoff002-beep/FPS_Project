// Fill out your copyright notice in the Description page of Project Settings.

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

enum class EButtonType : uint8;

UCLASS()
class FPS_PROJECT_API AFPSCharacter : public ACharacter, public IWeaponIKProvider, public ICharacterStateProvider
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSCharacter();

	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }

	FORCEINLINE FVector2D GetMouseInput() const { return MouseInputThisFrame; }

	FORCEINLINE UViewModelComponent* GetViewModelComponent() const { return ViewModel; }
	FORCEINLINE USkeletalMeshComponent* GetCharacterArms() const { return CharacterArms; }
	FORCEINLINE UWeaponManager* GetWeaponManager() { return WeaponManager; }
	FORCEINLINE UCharacterStatsDataAsset* GetCharacterStatsData() const { return CharacterStatsData; }

	FORCEINLINE bool IsWalking() const { return bWalk; }
	FORCEINLINE bool IsCrouching() const { return bCrouch; }
	FORCEINLINE bool IsAiming() const { return bAim; }

	FCharacterState GetCharacterState_Implementation() const;
	FViewModelState GetViewModelState_Implementation() const;

	void SetPlayerGameplayConfig(FGameplaySettingsData& NewData);

	FVector GetCameraRelativeLocation() const;
	
	FTransform GetRightHandIKTransform_Implementation() const;

	virtual FWeaponIKData GetWeaponIKData_Implementation() const override;

	void SetCameraRelativeLocation(FVector NewLocation);

	void BeginInteraction();
	void EndInteraction(float ElapsedTime);

	void ToggleBuildMode(bool bEnable);
	void SelectBuildingFromLibrary(int32 IntexToSelect);
	void PlaceBuildObject();
	void RotateBuildObject(float Direction);

	void ToggleExamination(bool& bExamed);

	UFUNCTION(BlueprintCallable)
		void TryGrabObject();

	UFUNCTION(BlueprintCallable)
		bool TryReleaseGrabbedObject();

	UFUNCTION(BlueprintCallable)
		bool TryThrowGrabbedObject();

	bool IsFalling() const;

	UFUNCTION(BlueprintCallable)
		void SetWalkState(bool bValue);

	UFUNCTION(BlueprintCallable)
		void SetSprintState(bool bValue);
	
	UFUNCTION(BlueprintCallable)
		void SetCrouchState(bool bValue);

	UFUNCTION(BlueprintCallable)
		void SetAimState(bool bValue);

	UFUNCTION(BlueprintCallable)
		bool IsSprinting() const;

	UFUNCTION(BlueprintCallable)
		bool IsCloseToWall() const;

	UFUNCTION(BlueprintCallable)
		bool IsReloading() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
		void MoveCharacter(const FVector2D& Axis);

	UFUNCTION(BlueprintCallable)
		void Look(const FVector2D& Axis);

	UFUNCTION(BlueprintCallable)
		void Shoot() const;

	UFUNCTION(BlueprintCallable)
		void SwapWeapon(const float Index) const;

	UFUNCTION(BlueprintCallable)
		void Reload() const;

	UFUNCTION(BlueprintCallable)
		void ToggleLaserAttachments() const;

	UFUNCTION(BlueprintCallable)
		void MoveWeaponStock() const;

	UPROPERTY(EditAnywhere, Category = "Interaction")\
		float InteractionMaxDistance = 250.f;

	UPROPERTY(EditAnywhere, Category = "Character Speed")
		float WalkSpeed = 600.f;

	UPROPERTY(EditAnywhere, Category = "Character Speed")
		float SprintSpeed = 1200.f;

	UPROPERTY(EditAnywhere, Category = "Character Height")
		float CrouchHalfHeight = 44.f;

	UPROPERTY(EditAnywhere, Category = "Character Height")
		float StandHalfHeight = 88.f;

	//UPROPERTY(EditAnywhere, Category = "Field Of View")
		//float WalkFOV = 90.f;

	UPROPERTY(EditAnywhere, Category = "Field Of View")
		float SprintFOV = 110.f;

	UPROPERTY(EditAnywhere, Category = "Acceleration")
		float HeightAccel = 10.f;

	UPROPERTY(EditAnywhere, Category = "Acceleration")
		float SpeedAccel = 10.f;

	UPROPERTY(EditAnywhere, Category = "Acceleration")
		float FOVAccel = 10.f;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
		//float MouseSensivity = 1.f;

	UPROPERTY(EditAnywhere)
		float DistanceToWall = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* Pivot = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UCameraComponent* Camera = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USkeletalMeshComponent* CharacterArms = nullptr;
		
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UViewModelComponent* ViewModel = nullptr;

	UPROPERTY(EditAnywhere)
		UWeaponManager* WeaponManager = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FName CameraSocket = NAME_None;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	FPlayerGameplayConfig PlayerGameplayConfig = {};

	UPROPERTY(EditAnywhere, Category = "Character Data")
		TObjectPtr<UCharacterStatsDataAsset> CharacterStatsData = nullptr;

	UPROPERTY() TObjectPtr<ULocalUIManager> UIManager = nullptr;
	UPROPERTY() TObjectPtr<UHandleGrabComponent> HandleGrabComponent = nullptr;

	UPROPERTY(EditAnywhere, Category = "Examination")
		TObjectPtr<UExaminationComponent> ExaminationComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Build")
		TObjectPtr<UConstructionComponent> ConstructionComponent = nullptr;

	UPROPERTY(EditAnywhere, Category = "Build Data")
		TObjectPtr<UBuildLibraryData> BuildLibraryData = nullptr;

	void UnifiedUIHandler(EButtonType ButtonType, UObject* Context);

	//void UpdateMovementSpeed(float DeltaTime);
	void UpdateCrouch(float DeltaTime);
	void UpdateFOV(float DeltaTime);

	void HandleInventoryChanged(int32 SlotIndex) const;
	void HandleSlotCleared(int32 SlotIndex);

	FHitResult* PerformInteraction();

	void HandleDropClickedItem(int32 SlotIndex);

	void SetDropLocation(FInventorySlot& ItemToDrop);

	void TryPickUpObject();

	bool bWalk = false;
	bool bSprint = false;
	bool bCrouch = false;
	bool bAim = false;
	bool bFalling = false;

	bool bHoldingObject = false;

	bool bIsInventoryOpen = false;

	FVector2D MouseInputThisFrame;

	UPROPERTY() TObjectPtr<USprintComponent> SprintComponent = nullptr;
	UPROPERTY() TObjectPtr<UStaminaComponent> StaminaComponent = nullptr;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UFPSWidget> UserHUDClass = nullptr;

	UPROPERTY(EditDefaultsOnly)
		TObjectPtr<USystemData> SystemData = nullptr;

	UPROPERTY(EditDefaultsOnly)
		TObjectPtr<UUIData> UIData = nullptr;

	UPROPERTY(VisibleAnywhere)
		UInventoryComponent* Inventory = nullptr;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UInventoryMenuWidget> InventoryWidgetClass = nullptr;

	UFUNCTION(BlueprintCallable)
		void DropItem();

	UFUNCTION(BlueprintCallable)
		void ToggleInventory();
};
