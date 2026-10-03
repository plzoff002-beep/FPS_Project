// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LaserAttachment.h"
#include "AttachmentTypes.h"
#include "AttachmentData.generated.h"

class ALaserAttachment;

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API UAttachmentData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Visual")
		TObjectPtr<UStaticMesh> AttachmentMesh;

	UPROPERTY(EditAnywhere, Category = "Visual")
		TSubclassOf<ALaserAttachment> AttachmentClass;

	UPROPERTY(EditAnywhere, Category = "Visual")
		FName TargetSocket;

	UPROPERTY(EditAnywhere, Category = "Logic")
		bool bAdjustable = false;

	UPROPERTY(EditAnywhere, Category = "Logic")
		FWeaponStatModifiers BasicModifiers;

	UPROPERTY(EditANywhere, Category = "Logic")
		FWeaponStatModifiers AdjustableModifiers;

	UPROPERTY(EditAnywhere, Category = "Logic")
		EAttachmentType AttachmentType = EAttachmentType::None;

	UPROPERTY(EditAnywhere, Category = "Logic")
		EAttachmentState CurrentState = EAttachmentState::None;

	FWeaponStatModifiers GetModifiers() const;

	void SetAttachmentState(EAttachmentState NewState);

	bool IsAttachmentExtended() const;
};
