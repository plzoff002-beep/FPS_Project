#include "FPSPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "InputTriggers.h"
#include "FPSCharacter.h"
#include "UITypes.h"
#include "EnhancedInputComponent.h"
#include "InteractionComponent.h"
#include "ExaminationComponent.h"
#include "HandleGrabComponent.h"
#include "ConstructionComponent.h"
#include "CrouchComponent.h"
#include "FPSStateComponent.h"
#include "SprintComponent.h"
#include "InventoryComponent.h"
#include "WeaponManager.h"
#include "LocalUIManager.h"

UEnhancedInputLocalPlayerSubsystem* AFPSPlayerController::GetEISubsystem() {
	if (ULocalPlayer* LP = GetLocalPlayer()) {
		return LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	}

	return nullptr;
}

void AFPSPlayerController::ToggleBuildMode() {
	if (!OwnerActor) return;

	if (!BuildMappingContext || !ConstructionComp) return;

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = GetEISubsystem()) {
		if (!bBuildModeActive) {
			Subsystem->AddMappingContext(BuildMappingContext, 1);
			ConstructionComp->ToggleBuildMode(!bBuildModeActive);
		}
		else {
			ConstructionComp->ToggleBuildMode(!bBuildModeActive);
			Subsystem->RemoveMappingContext(BuildMappingContext);
		}

		bBuildModeActive = !bBuildModeActive;
	}
}

void AFPSPlayerController::ToggleExaminationMode() {
	if (!ExamineMappingContext) return;

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = GetEISubsystem()) {
		if (bExamed) {
			Subsystem->AddMappingContext(ExamineMappingContext, 2);
		}
		else {
			Subsystem->RemoveMappingContext(ExamineMappingContext);
		}
	}
}

void AFPSPlayerController::HandleToggleInventory() {
	if (UIManager) {
		EUIType InventoryType = EUIType::Inventory;

		if (!UIManager->HasWidget(InventoryType) && InventoryComp) {
			UIManager->FindAndPushWidget(InventoryType, InventoryComp);
		}
		else {
			UIManager->FindAndPopWidget(InventoryType);
		}
	}
}

void AFPSPlayerController::HandleLook(const FInputActionValue& Value) {
	if (OwnerActor) {
		FVector2D LookDirection = Value.Get<FVector2D>();
		OwnerActor->Look(LookDirection);
	}
}

void AFPSPlayerController::HandleMove(const FInputActionValue& Value) {
	if (OwnerActor) {
		FVector2D MoveDirection = Value.Get<FVector2D>();
		OwnerActor->MoveCharacter(MoveDirection);
	}
}

void AFPSPlayerController::HandleToggleLaserAttachments() {
	if (WeaponManager) {
		WeaponManager->TryToggleLaserAttachments();
	}
}

void AFPSPlayerController::HandleReload() {
	if (WeaponManager) {
		WeaponManager->TryReload();
	}
}

void AFPSPlayerController::HandleAim() {
	if (StateComp) {
		StateComp->TryToggleAimed();
	}
}

void AFPSPlayerController::HandleShoot() {
	if (WeaponManager) {
		WeaponManager->TryShoot();
	}
}

void AFPSPlayerController::HandleThrowObject() {
	if (GrabComp = OwnerActor->FindComponentByClass<UHandleGrabComponent>()) {
		GrabComp->ThrowObject();
	}
}

void AFPSPlayerController::HandleReleaseObject() {
	if (GrabComp) {
		GrabComp->ReleaseObject();
	}
}

void AFPSPlayerController::HandleRotateExamine(const FInputActionValue& Value) {
	if (ExamComp) {
		ExamComp->RotateItem(Value.Get<FVector2D>());
	}
}

void AFPSPlayerController::HandleAdjustSceneCamera(const FInputActionValue& Value) {
	if (ExamComp) {
		ExamComp->AdjustSceneCamera(Value.Get<float>());
	}
}

void AFPSPlayerController::HandleInteract(const FInputActionInstance& Instance) {
	if (InteractionComp) {
		InteractionComp->EndInteraction(Instance.GetElapsedTime());
	}
}

void AFPSPlayerController::HandleExamineItem() {
	if (InteractionComp) {
		InteractionComp->TryExamineItem(bExamed);
		ToggleExaminationMode();
	}
}

void AFPSPlayerController::HandleRotateBuildObject(const FInputActionValue& Value) {
	if (!OwnerActor) return;

	if (UConstructionComponent* ConstructComp = OwnerActor->FindComponentByClass<UConstructionComponent>()) {
		ConstructComp->RotateBuildObject(Value.Get<float>());
	}
}

void AFPSPlayerController::HandlePlaceBuildObject() {
	if (ConstructionComp = OwnerActor->FindComponentByClass<UConstructionComponent>()) {
		ConstructionComp->PlaceBuildObject();
	}
}

void AFPSPlayerController::HandleDropItem() {
	if (InventoryComp) {
		if (WeaponManager) {
			int32 WeaponIndex = WeaponManager->GetCurrentWeaponIndex();
			if (WeaponIndex == INDEX_NONE) WeaponIndex = 0;

			InventoryComp->DropItem(WeaponIndex);
		}
	}
}

void AFPSPlayerController::HandleSelectItem(const FInputActionValue& Value) {
	if (InventoryComp) {
		const int32 Index = FMath::RoundToInt(Value.Get<float>() - 1);
			InventoryComp->SelectItem(Index);
	}
}

void AFPSPlayerController::HandleSelectSlot(const FInputActionValue& Value) {
	if (!OwnerActor) return;

	if (ConstructionComp) {
		int32 IndexToSelect = FMath::RoundToInt(Value.Get<float>());

		ConstructionComp->SelectBuildingFromLibrary(IndexToSelect - 1);
	}
}

void AFPSPlayerController::FindComponents(APawn* InPawn) {
	if (InPawn) {
		ExamComp = InPawn->FindComponentByClass<UExaminationComponent>();
		InteractionComp = InPawn->FindComponentByClass<UInteractionComponent>();
		GrabComp = InPawn->FindComponentByClass<UHandleGrabComponent>();
		ConstructionComp = InPawn->FindComponentByClass<UConstructionComponent>();
		CrouchComp = InPawn->FindComponentByClass<UCrouchComponent>();
		StateComp = InPawn->FindComponentByClass<UFPSStateComponent>();
		SprintComp = InPawn->FindComponentByClass<USprintComponent>();
		InventoryComp = InPawn->FindComponentByClass<UInventoryComponent>();
		WeaponManager = InPawn->FindComponentByClass<UWeaponManager>();

		if (ULocalPlayer* LP = GetLocalPlayer()) {
			UIManager = LP->GetSubsystem<ULocalUIManager>();
		}
	}
}

void AFPSPlayerController::BindInput() {
	if (UEnhancedInputComponent* EI = Cast<UEnhancedInputComponent>(InputComponent)) {
		EI->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AFPSPlayerController::HandleMove);
		EI->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AFPSPlayerController::HandleLook);
		EI->BindAction(IA_Jump, ETriggerEvent::Started, OwnerActor.Get(), &ACharacter::Jump);

		if (ExamComp) {
			EI->BindAction(IA_RotateExamine_Axis, ETriggerEvent::Triggered, this, &AFPSPlayerController::HandleRotateExamine);
			EI->BindAction(IA_AdjustSceneCamera, ETriggerEvent::Triggered, this, &AFPSPlayerController::HandleAdjustSceneCamera);
		}

		if (InteractionComp) {
			EI->BindAction(IA_Interact, ETriggerEvent::Completed, this, &AFPSPlayerController::HandleInteract);
			EI->BindAction(IA_ExamineItem, ETriggerEvent::Started, this, &AFPSPlayerController::HandleExamineItem);
		}

		if (GrabComp) {
			EI->BindAction(IA_ReleaseObject, ETriggerEvent::Started, this, &AFPSPlayerController::HandleReleaseObject);
			EI->BindAction(IA_ThrowObject, ETriggerEvent::Started, this, &AFPSPlayerController::HandleThrowObject);
		}

		if (ConstructionComp) {
			UConstructionComponent* CC = ConstructionComp;

			EI->BindAction(IA_ToggleBuildMode, ETriggerEvent::Started, this, &AFPSPlayerController::ToggleBuildMode);
			EI->BindAction(IA_SelectSlot, ETriggerEvent::Started, this, &AFPSPlayerController::HandleSelectSlot);
			EI->BindAction(IA_PlaceBuildObject, ETriggerEvent::Started, CC, &UConstructionComponent::PlaceBuildObject);
			EI->BindAction(IA_RotateBuildObject, ETriggerEvent::Triggered, this, &AFPSPlayerController::HandleRotateBuildObject);
		}

		if (WeaponManager) {
			UWeaponManager* WM = WeaponManager;
			EI->BindAction(IA_Shoot, ETriggerEvent::Triggered, WM, &UWeaponManager::TryShoot);
			EI->BindAction(IA_Reload, ETriggerEvent::Triggered, WM, &UWeaponManager::TryReload);
			EI->BindAction(IA_MoveStock, ETriggerEvent::Started, WM, &UWeaponManager::TryMoveWeaponStock);
			EI->BindAction(IA_ToggleLaserAttachments, ETriggerEvent::Triggered, WM, &UWeaponManager::TryToggleLaserAttachments);
		}

		if (StateComp) {
			EI->BindAction(IA_Aim, ETriggerEvent::Started, this, &AFPSPlayerController::HandleAim);
			EI->BindAction(IA_Aim, ETriggerEvent::Completed, this, &AFPSPlayerController::HandleAim);
		}

		if (CrouchComp) {
			UCrouchComponent* CC = CrouchComp;

			EI->BindAction(IA_Crouch, ETriggerEvent::Started, CC, &UCrouchComponent::ToggleCrouch);
			EI->BindAction(IA_Crouch, ETriggerEvent::Completed, CC, &UCrouchComponent::ToggleCrouch);
		}

		if (SprintComp) {
			USprintComponent* SC = SprintComp;

			EI->BindAction(IA_Sprint, ETriggerEvent::Started, SC, &USprintComponent::ToggleSprint);
			EI->BindAction(IA_Sprint, ETriggerEvent::Completed, SC, &USprintComponent::ToggleSprint);
		}

		if (InventoryComp) {
			EI->BindAction(IA_SelectItem, ETriggerEvent::Started, this, &AFPSPlayerController::HandleSelectItem);
			EI->BindAction(IA_DropItem, ETriggerEvent::Started, this, &AFPSPlayerController::HandleDropItem);
		}

		if (UIManager) {
			EI->BindAction(IA_ToggleInventory, ETriggerEvent::Started, this, &AFPSPlayerController::HandleToggleInventory);
		}
	}
}

void AFPSPlayerController::OnPossess(APawn* InPawn) {
	Super::OnPossess(InPawn);
	
	FindComponents(InPawn);
	BindInput();
}

void AFPSPlayerController::BeginPlay() {
	Super::BeginPlay();

	OwnerActor = GetPawn<AFPSCharacter>();

	if (InputMappingContext) {
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = GetEISubsystem()) {
			if (InputMappingContext) {
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}