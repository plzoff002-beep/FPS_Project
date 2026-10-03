// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BuildObjectData.generated.h"

class UStaticMesh;

UCLASS()
class FPS_PROJECT_API UBuildObjectData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Build")
		FName Name;
	
	UPROPERTY(EditAnywhere, Category = "Build")
		TSoftClassPtr<AActor> ActorClass = nullptr;

	UPROPERTY(EditAnywhere, Category = "Build")
		TSoftObjectPtr<UStaticMesh> GhostMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "Build")
		FVector GridSize = FVector(100.f, 100.f, 10.f);
};
