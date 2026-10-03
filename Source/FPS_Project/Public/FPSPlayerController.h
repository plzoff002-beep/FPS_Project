// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPSPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UEnhancedInputLocalPlayerSubsystem;

struct FInputActionInstance;
struct FInputActionValue;

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API AFPSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void BindOnCharacterReady();
	
protected:
	virtual void SetupInputComponent() override;
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

	UPROPERTY(EditAnywhere, Category = "Input Action")
		TObjectPtr<UInputAction> IA_SelectSlot = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input Action")
		TObjectPtr<UInputAction> IA_Interact = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input Action")
		TObjectPtr<UInputAction> IA_ReleaseObject = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input Action")
		TObjectPtr<UInputAction> IA_ThrowObject = nullptr;

private:
	UEnhancedInputLocalPlayerSubsystem* GetEISubsystem();

	void OnInteractStarted();
	void OnInteractCompleted(const FInputActionInstance& Instance);

	void OnThrowStarted();
	void OnReleaseStarted();

	void HandleSelectSlot(const FInputActionValue& Value);

	void ToggleBuildMode();
	void PlaceBuildObject();
	void RotateBuildObject(const FInputActionValue& Value);

	void ToggleExaminationMode();
	void TriggerExamine();

	bool bBuildModeActive = false;
	bool bExamed = false;
};
