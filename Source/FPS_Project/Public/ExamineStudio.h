// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExamineStudio.generated.h"

class UCameraComponent;

struct FExamineItem;

UCLASS()
class FPS_PROJECT_API AExamineStudio : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExamineStudio();

	void SetupItem(TSoftClassPtr<AActor> ItemToExamine);
	void RotateItem(const FVector2D& Delta);
	void AdjustSceneCamera(const float Value);

	FORCEINLINE void SetExamineData(FExamineItem* NewData) { CurrentData = NewData; }
	FORCEINLINE void SetActive(bool bEnable) { bActive = bEnable; }
	FORCEINLINE FExamineItem* GetCurrentData() const { return CurrentData; }

private:
	UPROPERTY(EditAnywhere) 
		TObjectPtr<UCameraComponent> SceneCamera = nullptr;

	UPROPERTY(EditAnywhere) 
		TObjectPtr<USceneComponent> ItemAnchor = nullptr;

	TSoftClassPtr<AActor> CurrentClassToLoad = nullptr;

	UPROPERTY() TObjectPtr<AActor> CurrentExamineActor = nullptr;
		
	FExamineItem* CurrentData;

	void LoadItemClassAsync(TSoftClassPtr<AActor> ClassToLoad);
	void SetupItemOnLoaded(TSubclassOf<AActor> ClassToSetup);

	void ResetItemAnchor();
	void ResetSceneCamera();

	void AdjustCameraDistanceLimits();

	bool bActive = false;

	UPROPERTY(EditAnywhere, Category = "Settings | Camera Adjusting")
		float AdjustingRate = 10.f;

	UPROPERTY(EditAnywhere, Category = "Settings | Camera Adjusting")
		float SensitivityRotation = 1.f;
};
