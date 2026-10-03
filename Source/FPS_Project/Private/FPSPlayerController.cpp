// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputTriggers.h"
#include "FPSCharacter.h"
#include "ExaminationComponent.h"

UEnhancedInputLocalPlayerSubsystem* AFPSPlayerController::GetEISubsystem() {
	if (ULocalPlayer* LP = GetLocalPlayer()) {
		return LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	}

	return nullptr;
}

void AFPSPlayerController::ToggleBuildMode() {
	if (!BuildMappingContext) return;

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = GetEISubsystem()) {
		if (!bBuildModeActive) {
			Subsystem->AddMappingContext(BuildMappingContext, 1);

			if (AFPSCharacter* MyChar = GetPawn<AFPSCharacter>()) {
				MyChar->ToggleBuildMode(!bBuildModeActive);
			}
		}
		else {
			if (AFPSCharacter* MyChar = GetPawn<AFPSCharacter>()) {
				MyChar->ToggleBuildMode(!bBuildModeActive);
				Subsystem->RemoveMappingContext(BuildMappingContext);
			}
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

void AFPSPlayerController::TriggerExamine() {
	if (AFPSCharacter* MyChar = GetPawn<AFPSCharacter>()) {
		MyChar->ToggleExamination(bExamed);
		ToggleExaminationMode();
	}
}

void AFPSPlayerController::RotateBuildObject(const FInputActionValue& Value) {
	if (AFPSCharacter* MyChar = GetPawn<AFPSCharacter>()) {
		MyChar->RotateBuildObject(Value.Get<float>());
	}
}

void AFPSPlayerController::PlaceBuildObject() {
	if (AFPSCharacter* MyChar = GetPawn<AFPSCharacter>()) {
		MyChar->PlaceBuildObject();
	}
}

void AFPSPlayerController::HandleSelectSlot(const FInputActionValue& Value) {
	int32 IndexToSelect = FMath::RoundToInt(Value.Get<float>());

	if (AFPSCharacter* MyChar = GetPawn<AFPSCharacter>()) {
		MyChar->SelectBuildingFromLibrary(IndexToSelect - 1);
	}
}

void AFPSPlayerController::OnThrowStarted() {
	if (AFPSCharacter* MyChar = GetPawn<AFPSCharacter>()) {
		MyChar->TryThrowGrabbedObject();
	}
}

void AFPSPlayerController::OnReleaseStarted() {
	if (AFPSCharacter* MyChar = GetPawn<AFPSCharacter>()) {
		MyChar->TryReleaseGrabbedObject();
	}
}

void AFPSPlayerController::OnInteractCompleted(const FInputActionInstance& Instance) {
	if (AFPSCharacter* MyChar = GetPawn<AFPSCharacter>()) {
		MyChar->EndInteraction(Instance.GetElapsedTime());
	}
}

void AFPSPlayerController::OnInteractStarted() {
	if (AFPSCharacter* MyChar = GetPawn<AFPSCharacter>()) {
		MyChar->BeginInteraction();
	}
}

void AFPSPlayerController::OnPossess(APawn* InPawn) {
	Super::OnPossess(InPawn);
	
	if (AFPSCharacter* MyChar = Cast<AFPSCharacter>(InPawn)) {
		if (UEnhancedInputComponent* EI = Cast<UEnhancedInputComponent>(InputComponent)) {
			if (UExaminationComponent* ExamComp = MyChar->FindComponentByClass<UExaminationComponent>()) {
				EI->BindAction(IA_ExamineItem, ETriggerEvent::Started, this, &AFPSPlayerController::TriggerExamine);
				EI->BindAction(IA_RotateExamine_Axis, ETriggerEvent::Triggered, ExamComp, &UExaminationComponent::RotateItem);
				EI->BindAction(IA_AdjustSceneCamera, ETriggerEvent::Triggered, ExamComp, &UExaminationComponent::AdjustSceneCamera);
			}
		}
	}
}

void AFPSPlayerController::BeginPlay() {
	if (!InputMappingContext) return;

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = GetEISubsystem()) {
		if (InputMappingContext) {
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}
}

void AFPSPlayerController::SetupInputComponent() {
	Super::SetupInputComponent();


	if (UEnhancedInputComponent* EI = Cast<UEnhancedInputComponent>(InputComponent)) {
		EI->BindAction(IA_Interact, ETriggerEvent::Started, this, &AFPSPlayerController::OnInteractStarted);
		EI->BindAction(IA_Interact, ETriggerEvent::Completed, this, &AFPSPlayerController::OnInteractCompleted);

		EI->BindAction(IA_ReleaseObject, ETriggerEvent::Started, this, &AFPSPlayerController::OnReleaseStarted);
		EI->BindAction(IA_ThrowObject, ETriggerEvent::Started, this, &AFPSPlayerController::OnThrowStarted);

		EI->BindAction(IA_SelectSlot, ETriggerEvent::Started, this, &AFPSPlayerController::HandleSelectSlot);
		EI->BindAction(IA_ToggleBuildMode, ETriggerEvent::Started, this, &AFPSPlayerController::ToggleBuildMode);
		EI->BindAction(IA_PlaceBuildObject, ETriggerEvent::Started, this, &AFPSPlayerController::PlaceBuildObject);
		EI->BindAction(IA_RotateBuildObject, ETriggerEvent::Started, this, &AFPSPlayerController::RotateBuildObject);
	}
}