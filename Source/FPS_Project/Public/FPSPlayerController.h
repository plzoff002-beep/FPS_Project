#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPSPlayerController.generated.h"

class AFPSCharacter;
class UInputMappingContext;
class UInputAction;
class UEnhancedInputLocalPlayerSubsystem;

class UExaminationComponent;
class UInteractionComponent;
class UHandleGrabComponent;
class UConstructionComponent;
class UCrouchComponent;
class UFPSStateComponent;
class USprintComponent;
class UInventoryComponent;
class UWeaponManager;
class ULocalUIManager;

struct FInputActionInstance;
struct FInputActionValue;

UCLASS()
class FPS_PROJECT_API AFPSPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY(EditAnywhere, Category = "Mapping Context")
		TObjectPtr<UInputMappingContext> InputMappingContext = nullptr;	

	UPROPERTY(EditAnywhere, Category = "Mapping Context")
		TObjectPtr<UInputMappingContext> BuildMappingContext = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mapping Context")
		TObjectPtr<UInputMappingContext> ExamineMappingContext = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input Action | Build")
		TObjectPtr<UInputAction> IA_PlaceBuildObject = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input Action | Build")
		TObjectPtr<UInputAction> IA_ToggleBuildMode = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input Action | Build")
		TObjectPtr<UInputAction> IA_RotateBuildObject = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input Action | Examine")
		TObjectPtr<UInputAction> IA_ExamineItem = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input Action | Examine")
		TObjectPtr<UInputAction> IA_AdjustSceneCamera = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input Action | Examine")
		TObjectPtr<UInputAction> IA_RotateExamine_Click = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input Action | Examine")
		TObjectPtr<UInputAction> IA_RotateExamine_Axis = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input Action | Weapon")
		TObjectPtr<UInputAction> IA_Shoot = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input Action | Weapon")
		TObjectPtr<UInputAction> IA_Aim = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input Action | Weapon")
		TObjectPtr<UInputAction> IA_Reload = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input Action | Weapon")
		TObjectPtr<UInputAction> IA_MoveStock = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input Action | Weapon")
		TObjectPtr<UInputAction> IA_ToggleLaserAttachments = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input Action | Inventory")
		TObjectPtr<UInputAction> IA_DropItem = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input Action | Inventory")
		TObjectPtr<UInputAction> IA_SelectItem = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input Action")
		TObjectPtr<UInputAction> IA_SelectSlot = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input Action")
		TObjectPtr<UInputAction> IA_Interact = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input Action")
		TObjectPtr<UInputAction> IA_ReleaseObject = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input Action")
		TObjectPtr<UInputAction> IA_ThrowObject = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input Action")
		TObjectPtr<UInputAction> IA_Crouch = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input Action")
		TObjectPtr<UInputAction> IA_Sprint = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input Action")
		TObjectPtr<UInputAction> IA_Jump = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input Action")
		TObjectPtr<UInputAction> IA_Move = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input Action")
		TObjectPtr<UInputAction> IA_Look = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input Action")
		TObjectPtr<UInputAction> IA_ToggleInventory = nullptr;

private:
	UPROPERTY() TObjectPtr<AFPSCharacter> OwnerActor = nullptr;

	UPROPERTY() TObjectPtr<UExaminationComponent> ExamComp = nullptr;
	UPROPERTY() TObjectPtr<UInteractionComponent> InteractionComp = nullptr;
	UPROPERTY() TObjectPtr<UHandleGrabComponent> GrabComp = nullptr;
	UPROPERTY() TObjectPtr<UConstructionComponent> ConstructionComp = nullptr;
	UPROPERTY() TObjectPtr<UWeaponManager> WeaponManager = nullptr;
	UPROPERTY() TObjectPtr<ULocalUIManager> UIManager = nullptr;
	UPROPERTY() TObjectPtr<UCrouchComponent> CrouchComp = nullptr;
	UPROPERTY() TObjectPtr<UFPSStateComponent> StateComp = nullptr;
	UPROPERTY() TObjectPtr<USprintComponent> SprintComp = nullptr;
	UPROPERTY() TObjectPtr<UInventoryComponent> InventoryComp = nullptr;

	UEnhancedInputLocalPlayerSubsystem* GetEISubsystem();

	void FindComponents(APawn* InPawn);
	void BindInput();

	void HandleInteract(const FInputActionInstance& Instance);

	void HandleSelectSlot(const FInputActionValue& Value);

	void ToggleBuildMode();
	void HandlePlaceBuildObject();
	void HandleRotateBuildObject(const FInputActionValue& Value);

	void ToggleExaminationMode();
	void HandleRotateExamine(const FInputActionValue& Value);
	void HandleAdjustSceneCamera(const FInputActionValue& Value);
	void HandleExamineItem();

	void HandleReleaseObject();
	void HandleThrowObject();

	void HandleShoot();
	void HandleAim();
	void HandleReload();
	void HandleToggleLaserAttachments();

	void HandleMove(const FInputActionValue& Value);
	void HandleLook(const FInputActionValue& Value);

	void HandleSelectItem(const FInputActionValue& Value);
	void HandleDropItem();

	void HandleToggleInventory();

	bool bBuildModeActive = false;
	bool bExamed = false;
	bool bCrouch = false;
};
